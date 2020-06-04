#include "../shared/protobuf/cpp/ResocaMessage.pb.h"
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <functional>

#define PROTOBUF_MAX_DATA_LEN 1024

class TCPServer;

class TCPSession {
public:
    TCPSession(uint32_t sid, boost::asio::ip::tcp::socket socket, TCPServer &server):
        sid(sid), socket(std::move(socket)), server(server) {}

    void start();

    void writeRSM(std::shared_ptr<ResocaMessage> msg);

    bool handlePBMessage(std::shared_ptr<ResocaMessage> rms);

    std::set<std::string> ifNotify;

    void kill();

private:
    uint32_t sid;
    boost::asio::ip::tcp::socket socket;
    TCPServer &server;

    uint16_t lengthData[1];
    uint8_t data[PROTOBUF_MAX_DATA_LEN];

};

class TCPServer {
public:
    TCPServer(std::string interface, int portNumber, boost::asio::io_context &ioContext)
        : interface(interface), portNumber(portNumber),
            acceptor(ioContext,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), portNumber)) {}
    //boost::asio::ip::address::from_string(interface)

    void listen();

    bool handlePBMessage(std::shared_ptr<ResocaMessage> rms);

    void setHandleReceivePBMessage(std::function<bool(std::shared_ptr<ResocaMessage> rms)> func) {
        handleReceivePBMessage = func;
    }

    void deleteSession(int sid);

private:

    // next session id to be assigned.
    uint32_t nextSid = 1;
    std::map<uint32_t,TCPSession*> sessions;

    std::string interface;
    int portNumber;
    //boost::asio::io_context &ioContext;

    boost::asio::ip::tcp::acceptor acceptor;

    std::function<bool(std::shared_ptr<ResocaMessage> rms)> handleReceivePBMessage;
};