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
                    BOOST_LOG_TRIVIAL(error) << "Other error:"
                    << errorCode << " or failed to read correct length: "
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

bool TCPSession::handlePBMessage(std::shared_ptr<ResocaMessage> rms) {

    return true;
}

void TCPServer::deleteSession(int sid) {
    if(sessions.count(sid)) {
        auto s = sessions[sid];
        sessions.erase(sid);
        delete s;
        BOOST_LOG_TRIVIAL(debug) << "Deleted session with sid: " << sid;
    } else {
        BOOST_LOG_TRIVIAL(debug) << "Session with sid " << sid << " does not exist.";
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
            } else {
                BOOST_LOG_TRIVIAL(error) << "Error in listen(): " << errorCode.message();
            }
            listen();
    });
}

bool TCPServer::handlePBMessage(std::shared_ptr<ResocaMessage> rms) {
    BOOST_LOG_TRIVIAL(debug) << "Handling PBMessage in TCPServer: " << rms->DebugString();
    return true;
}

