#pragma once
#include "config.h"
#include "../shared/protobuf/cpp/ResocaMessage.pb.h"
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <functional>

#include "CanDeviceManager.hpp"

#define PROTOBUF_MAX_DATA_LEN 1024
#define MAX_N_UNSENT 128

class TCPServer;
class CanDeviceManager;

class TCPSession {
public:
    TCPSession(uint32_t sid, boost::asio::ip::tcp::socket socket, TCPServer &server):
        sid(sid), socket(std::move(socket)), server(server) {}

    void start();

    void writeRSM(ResocaMessage &msg);

    bool handlePBMessage(ResocaMessage &rms);

    std::set<std::string> ifNotify;

    void kill();

private:
    uint32_t sid;
    boost::asio::ip::tcp::socket socket;
    TCPServer &server;

    int nUnsentRSM = 0;

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

    bool sendPBMessage(ResocaMessage &rms);

    void deleteSession(int sid);

    std::vector<std::string> getIfList();

    void setCDM(CanDeviceManager *cdm) {
        this->cdm = cdm;
    }

    bool handleCanEvent(CanEvent &ce);

private:

    CanDeviceManager *cdm;


    // next session id to be assigned.
    uint32_t nextSid = 1;
    std::map<uint32_t,TCPSession*> sessions;

    std::string interface;
    int portNumber;

    boost::asio::ip::tcp::acceptor acceptor;
};
