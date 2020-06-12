#include "TCPServer.hpp"

void TCPSession::kill() {
    BOOST_LOG_TRIVIAL(debug) << "Requesting destruction of session";
    server.deleteSession(sid);
}

void TCPSession::start() {
    BOOST_LOG_TRIVIAL(trace) << "start on session: " << sid;
    async_read(socket, boost::asio::buffer(lengthData),
    [=](const boost::system::error_code &errorCode, size_t bytes){
        if (errorCode || bytes != 2) {
            BOOST_LOG_TRIVIAL(error) << "Failed to read length";
            kill();
        } else {
            auto messageLength = lengthData[0];
            BOOST_LOG_TRIVIAL(debug) << "Receiving message length: " << messageLength;
            auto dataBuf = boost::asio::buffer(data, messageLength);
            async_read(socket, dataBuf,
            [=](const boost::system::error_code &errorCode, size_t bytes){
                if (errorCode || bytes != messageLength) {
                    BOOST_LOG_TRIVIAL(error) << "Error reading body: "
                    << errorCode.message();
                    kill();
                } else if(bytes != messageLength) {
                    BOOST_LOG_TRIVIAL(error) <<"failed to read correct length: "
                    << bytes << " instead of: "
                    << messageLength;
                    kill();
                } else {
                    // Deserialize data
                    auto rms = std::make_shared<ResocaMessage>();
                    if(!rms->ParseFromArray(data, messageLength)) {
                        BOOST_LOG_TRIVIAL(error) << "Failed to deserialize Protobuf stream";
                        kill();
                    } else {
                        handlePBMessage(rms);
                        start();
                    }
                }
            });
        }
    });
}

bool TCPSession::handlePBMessage(std::shared_ptr<ResocaMessage> rsm) {
    BOOST_LOG_TRIVIAL(debug) << "Handling RSM: " << rsm->DebugString();
    if(!rsm->isresponse()) {
        if(rsm->request().requesttype() == ResocaMessage_Request_RequestType_INFO) {
                BOOST_LOG_TRIVIAL(debug) << "Info Request";
                auto respMsg = std::make_shared<ResocaMessage>();
                auto resp = new ResocaMessage_Response();

                resp->set_responsetype(ResocaMessage_Response_ResponseType_INFO);
                auto info = new ResocaMessage_Response_ResocaInfo();
                info->set_version(VERSION);
                info->set_sessionprefix(sid << 16);
                for(auto itf : server.getIfList()) {
                    info->add_interfaces(itf);
                }
                resp->set_allocated_resocainfo(info);
                respMsg->set_allocated_response(resp);
                writeRSM(respMsg);
                return true;

        } else if (!rsm->request().requestid()) {
                auto respMsg = std::make_shared<ResocaMessage>();
                respMsg->set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                resp->set_description("No RequestID specified.");
                respMsg->set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
        } else if ((rsm->request().requestid() >> 16) != sid) {
                auto respMsg = std::make_shared<ResocaMessage>();
                respMsg->set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                resp->set_description("RequestID SID mask invalid.");
                respMsg->set_allocated_response(resp);

                //BOOST_LOG_TRIVIAL(debug) << "RESPONSE: " << respMsg->DebugString();

                writeRSM(respMsg);
                return true;
        }

        switch(rsm->request().requesttype()) {
            case ResocaMessage_Request_RequestType_PING: {
                auto respMsg = std::make_shared<ResocaMessage>();
                respMsg->set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_PONG);
                resp->set_responseid(rsm->request().requestid());
                respMsg->set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
            }
            case ResocaMessage_Request_RequestType_NOTIFY_START: {

            }
            case ResocaMessage_Request_RequestType_NOTIFY_END: {

            }
            default: {
            }

        }

    } else {
        writeRSM(rsm);
    }
    return true;
}

void TCPSession::writeRSM(std::shared_ptr<ResocaMessage> msg) {
    unsigned short len = msg->ByteSizeLong();
    BOOST_LOG_TRIVIAL(debug) << "Serialized rsm is " << len << " bytes long";
    uint8_t msgData[2+len];
    *(unsigned short *) msgData = len;
    msg->SerializeToArray(&(msgData[2]), len);
    std::stringstream ss;
    for(uint8_t a : msgData) {
        ss << std::hex << (int) a << " ";
    }

    BOOST_LOG_TRIVIAL(debug) << "SENDING: " << ss.str();

    boost::asio::async_write(socket, boost::asio::buffer(msgData,2+len),
        [this](const boost::system::error_code &errorCode, size_t bytes){
            if(errorCode) {
                BOOST_LOG_TRIVIAL(error)
                << "Failed to write message to client, terminating session. error: "
                << errorCode.message();
                kill();
            } else {
                BOOST_LOG_TRIVIAL(debug) << "Successfully send message (" << bytes << " bytes) to client.";
            }
        });

}

void TCPServer::deleteSession(int sid) {
    if(sessions.count(sid)) {
        auto s = sessions[sid];
        sessions.erase(sid);
        delete s;
        BOOST_LOG_TRIVIAL(debug) << "Deleted session with sid: " << sid;
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Session with sid " << sid << " does not exist. known sessions (" << sessions.size() <<") :";
        for( const auto &pair : sessions) {
            BOOST_LOG_TRIVIAL(debug) << "- " << pair.first;
        }
    }
}

void TCPServer::listen() {
    BOOST_LOG_TRIVIAL(info) << "Starting to listen on " << interface << ":" << portNumber;
    acceptor.async_accept(
        [this](boost::system::error_code errorCode, boost::asio::ip::tcp::socket socket) {
            if (!errorCode)
            {
                auto ts = new TCPSession(nextSid, std::move(socket), *this);
                sessions[nextSid] = ts;
                ts->start();
                nextSid ++;
            } else {
                BOOST_LOG_TRIVIAL(error) << "Error in listen(): " << errorCode.message();
            }
            listen();
    });
}

bool TCPServer::sendPBMessage(std::shared_ptr<ResocaMessage> rsm) {
    BOOST_LOG_TRIVIAL(debug) << "sending PBMessage in TCPServer: " << rsm->DebugString();
    for(auto &pair : sessions) {
        auto session = pair.second;
        if(rsm->response().ifname() == "" || session->ifNotify.find(rsm->response().ifname()) != session->ifNotify.end()) {
            BOOST_LOG_TRIVIAL(trace) << "Sent message to session: " << pair.first;
            session->writeRSM(rsm);
        }
    }
    return true;
}
std::vector<std::string> TCPServer::getIfList() {
    return cdm->getIfList();
}
