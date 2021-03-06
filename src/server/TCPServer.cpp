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
            //auto dataBuf = boost::asio::buffer(data, messageLength);
            async_read(socket, boost::asio::buffer(data, messageLength),
            [=](const boost::system::error_code &errorCode, size_t bytes){
                //delete dataBuf;
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
                    ResocaMessage rsm;
                    if(!rsm.ParseFromArray(data, messageLength)) {
                        BOOST_LOG_TRIVIAL(error) << "Failed to deserialize Protobuf stream";
                        kill();
                    } else {
                        handlePBMessage(rsm);
                        start();
                    }
                }
            });
        }
    });
}

bool TCPSession::handlePBMessage(ResocaMessage &rsm) {
    BOOST_LOG_TRIVIAL(debug) << "Handling RSM: " << rsm.DebugString();
    if(!rsm.isresponse()) {
        if(rsm.request().requesttype() == ResocaMessage_Request_RequestType_INFO) {
                BOOST_LOG_TRIVIAL(debug) << "Info Request";
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();

                resp->set_responsetype(ResocaMessage_Response_ResponseType_INFO);
                auto info = new ResocaMessage_Response_ResocaInfo();
                info->set_version(VERSION);
                info->set_sessionprefix(sid << 16);
                for(auto itf : server.getIfList()) {
                    info->add_interfaces(itf);
                }
                resp->set_allocated_resocainfo(info);
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;

        } else if (!rsm.request().requestid()) {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                resp->set_description("No RequestID specified.");
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
        } else if ((rsm.request().requestid() >> 16) != sid) {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                resp->set_description("RequestID SID mask invalid.");
                respMsg.set_allocated_response(resp);

                //BOOST_LOG_TRIVIAL(debug) << "RESPONSE: " << respMsg.DebugString();

                writeRSM(respMsg);
                return true;
        }

        switch(rsm.request().requesttype()) {
            case ResocaMessage_Request_RequestType_PING: {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();
                resp->set_responsetype(ResocaMessage_Response_ResponseType_PONG);
                resp->set_responseid(rsm.request().requestid());
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
            }
            case ResocaMessage_Request_RequestType_NOTIFY_START: {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();

                std::string ifn = rsm.request().ifname();
                auto exists = ifNotify.count(ifn) == 0;

                if(exists) {
                    ifNotify.insert(ifn);
                    BOOST_LOG_TRIVIAL(debug) << "NOTIFY START ON: " << ifn;
                } else {
                    BOOST_LOG_TRIVIAL(debug) << "NOTIFY START ON (was already listening): " << ifn;
                    resp->set_description("Was already notifying.");
                }

                resp->set_responsetype(ResocaMessage_Response_ResponseType_SUCCESS);
                resp->set_responseid(rsm.request().requestid());
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
            }
            case ResocaMessage_Request_RequestType_NOTIFY_END: {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();

                std::string ifn = rsm.request().ifname();
                auto exists = ifNotify.count(ifn) > 0;

                if(exists) {
                    ifNotify.erase(ifn);
                    BOOST_LOG_TRIVIAL(debug) << "NOTIFY END ON: " << ifn;
                } else {
                    BOOST_LOG_TRIVIAL(debug) << "NOTIFY END ON (was not listening): " << ifn;
                    resp->set_description("Was not notifying.");
                }

                resp->set_responsetype(ResocaMessage_Response_ResponseType_SUCCESS);
                resp->set_responseid(rsm.request().requestid());
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
            }
            case ResocaMessage_Request_RequestType_CAN_TX: {
                ResocaMessage respMsg;
                respMsg.set_isresponse(true);
                auto resp = new ResocaMessage_Response();

                auto rscf = rsm.request().canframe();

                if(!rsm.request().ifname().length()) {
                    resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                    resp->set_responseid(rsm.request().requestid());
                    resp->set_description("No ifname provided");
                    respMsg.set_allocated_response(resp);
                    writeRSM(respMsg);
                    return false;
                }
                CanEvent ce(rsm.request().ifname(), FRAME_TX);
                ce.id = rsm.request().requestid();
                ce.sent = false;
                try {
                    auto cf = new CanFrame();
                    cf->canID = (uint32_t)rscf.canid();
                    cf->isEFFFrame = rscf.iseffframe();
                    cf->isRTRFrame = rscf.isrtrframe();
                    cf->isERRFrame = rscf.iserrframe();
                    cf->isCanFd = rscf.iscanfd();
                    cf->isCanFdESI = rscf.iscanfdesi();
                    cf->isCanFdBRS = rscf.iscanfdbrs();

                    auto dataStr = rscf.data();
                    cf->copyData((uint8_t *)dataStr.c_str(), dataStr.length());
                    ce.canFrame = cf;

                BOOST_LOG_TRIVIAL(trace) << "TCPSESSION: constructed CanEvent: " << ce.toString();
                } catch(...) {
                    BOOST_LOG_TRIVIAL(error) << "Error deserializing";
                    resp->set_responsetype(ResocaMessage_Response_ResponseType_ERR);
                    resp->set_responseid(rsm.request().requestid());
                    resp->set_description("Error deserializing CanFrame");
                    respMsg.set_allocated_response(resp);
                    writeRSM(respMsg);
                    return false;
                }

                bool success = server.handleCanEvent(ce);

                resp->set_responsetype(
                        success ? ResocaMessage_Response_ResponseType_ACK
                        : ResocaMessage_Response_ResponseType_ERR);
                resp->set_responseid(rsm.request().requestid());
                respMsg.set_allocated_response(resp);
                writeRSM(respMsg);
                return true;
            }
            default: {
                BOOST_LOG_TRIVIAL(warning) << "Unknown Message";
            }

        }

    } else {
        BOOST_LOG_TRIVIAL(warning) << "We don't deal with no responses 'round here!";
        writeRSM(rsm);
    }
    return true;
}

void TCPSession::writeRSM(ResocaMessage &msg) {
    unsigned short len = msg.ByteSizeLong();
    BOOST_LOG_TRIVIAL(debug) << "Serialized rsm is " << len << " bytes long";
    uint8_t msgData[2+len];
    *(unsigned short *) msgData = len;
    msg.SerializeToArray(&(msgData[2]), len);
    std::stringstream ss;
    for(uint8_t a : msgData) {
        ss << std::hex << (int) a << " ";
    }
    
    if(nUnsentRSM >= MAX_N_UNSENT){
        BOOST_LOG_TRIVIAL(error) << "Could not send, too many are unsent: " << nUnsentRSM;
        return;
    }

    nUnsentRSM ++;

    BOOST_LOG_TRIVIAL(debug) << "SENDING: " << ss.str();

    boost::asio::async_write(socket, boost::asio::buffer(msgData,2+len),
        [this](const boost::system::error_code &errorCode, size_t bytes){
            nUnsentRSM--;
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

bool TCPServer::sendPBMessage(ResocaMessage &rsm) {
    BOOST_LOG_TRIVIAL(debug) << "sending PBMessage in TCPServer: " << rsm.DebugString();
    for(auto &pair : sessions) {
        auto session = pair.second;
        if(rsm.response().ifname() == "" || session->ifNotify.find(rsm.response().ifname()) != session->ifNotify.end()) {
            BOOST_LOG_TRIVIAL(trace) << "Sent message to session: " << pair.first;
            session->writeRSM(rsm);
            BOOST_LOG_TRIVIAL(debug) << "POST writeRSM for session";
        }
    }
    return true;
}

std::vector<std::string> TCPServer::getIfList() {
    return cdm->getIfList();
}

bool TCPServer::handleCanEvent(CanEvent &ce) {
    switch(ce.eventType) {
        case FRAME_TX: {
            if(!ce.sent){
                BOOST_LOG_TRIVIAL(trace) << "TCPSERVER: passing CanEvent to CDM: " << ce.toString();
                return cdm->handleCanEvent(ce);
            }
            BOOST_LOG_TRIVIAL(trace) << "TCPSERVER: handling CanEvent: " << ce.toString();

            ResocaMessage respMsg;
            respMsg.set_isresponse(true);
            auto resp = new ResocaMessage_Response();
            if(!ce.err){
                resp->set_responsetype(ResocaMessage_Response_ResponseType_CAN_TX);
            } else {
                resp->set_responsetype(ResocaMessage_Response_ResponseType_CAN_TX_ERR);
                resp->set_description(std::to_string(ce.err));
            }
            resp->set_responseid(ce.id);
            auto cf = new ResocaMessage_CanFrame();
            cf->set_canid(ce.canFrame->canID);
            cf->set_iscanfd(ce.canFrame->isCanFd);
            cf->set_iseffframe(ce.canFrame->isEFFFrame);
            cf->set_isrtrframe(ce.canFrame->isRTRFrame);
            cf->set_iserrframe(ce.canFrame->isERRFrame);
            cf->set_iscanfdesi(ce.canFrame->isCanFdESI);
            cf->set_iscanfdbrs(ce.canFrame->isCanFdBRS);
            cf->set_data((const char *)ce.canFrame->data, ce.canFrame->length);

            resp->set_allocated_canframe(cf);
            resp->set_ifname(ce.ifName);
            respMsg.set_allocated_response(resp);

            BOOST_LOG_TRIVIAL(trace) << "Wrote response for FRAME_TX";
            sendPBMessage(respMsg);
            BOOST_LOG_TRIVIAL(debug) << "POST sendPBMessage";
            return true;
        }
        case FRAME_RX: {
            ResocaMessage respMsg;
            respMsg.set_isresponse(true);
            auto resp = new ResocaMessage_Response();
            resp->set_responsetype(ResocaMessage_Response_ResponseType_CAN_RX);
            auto cf = new ResocaMessage_CanFrame();
            cf->set_canid(ce.canFrame->canID);
            cf->set_iscanfd(ce.canFrame->isCanFd);
            cf->set_iseffframe(ce.canFrame->isEFFFrame);
            cf->set_isrtrframe(ce.canFrame->isRTRFrame);
            cf->set_iserrframe(ce.canFrame->isERRFrame);
            cf->set_iscanfdesi(ce.canFrame->isCanFdESI);
            cf->set_iscanfdbrs(ce.canFrame->isCanFdBRS);
            cf->set_data((const char *)ce.canFrame->data, ce.canFrame->length);

            resp->set_allocated_canframe(cf);
            resp->set_ifname(ce.ifName);
            respMsg.set_allocated_response(resp);

            BOOST_LOG_TRIVIAL(debug) << "Message created for CAN_RX: "
                << respMsg.DebugString();
            sendPBMessage(respMsg);
            return true;
        }
        case IF_CONNECTED: {
            ResocaMessage respMsg;
            respMsg.set_isresponse(true);
            auto resp = new ResocaMessage_Response();
            resp->set_responsetype(ResocaMessage_Response_ResponseType_CAN_IF_CONNECTED);
            resp->set_ifname(ce.ifName);
            respMsg.set_allocated_response(resp);

            BOOST_LOG_TRIVIAL(debug) << "Message created for IF_CONNECTED: "
                << respMsg.DebugString();
            sendPBMessage(respMsg);
            return true;
        }
        case IF_DISCONNECTED: {
            ResocaMessage respMsg;
            respMsg.set_isresponse(true);
            auto resp = new ResocaMessage_Response();
            resp->set_responsetype(ResocaMessage_Response_ResponseType_CAN_IF_DISCONNECTED);
            resp->set_ifname(ce.ifName);
            respMsg.set_allocated_response(resp);

            BOOST_LOG_TRIVIAL(debug) << "Message created for IF_DISCONNECTED: "
                << respMsg.DebugString();
            sendPBMessage(respMsg);
            return true;
        }
        default: {
            BOOST_LOG_TRIVIAL(error) << "NOT IMPLEMENTED: " << (int) ce.eventType;
            return false;
        }
    }
}

bool CanDeviceManager::handleCanEvent(CanEvent &ce) {
    if(ce.eventType == FRAME_TX && !ce.sent) {
        BOOST_LOG_TRIVIAL(debug) << "CDM: Handling CanEvent: " << ce.toString();
        if(!canDevices.count(ce.ifName)) {
            BOOST_LOG_TRIVIAL(error) << "CDM: Unknown interface: " << ce.ifName;
            return false;
        }
        BOOST_LOG_TRIVIAL(trace) << "CDM: CALLING SENDFRAME";
        canDevices[ce.ifName]->sendFrame(*ce.canFrame, ce.id);
        return true;
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Passing CanEvent to server: " << ce.toString();
        server->handleCanEvent(ce);
    }
    return true;
}
