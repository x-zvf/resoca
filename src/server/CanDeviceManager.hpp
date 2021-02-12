#pragma once

#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <functional>

#include "CanDevice.hpp"
#include "../shared/protobuf/cpp/ResocaMessage.pb.h"
#include "CanFrame.hpp"
#include "CanEvent.hpp"

class TCPServer;

class CanDeviceManager {
public:
    CanDeviceManager(boost::asio::io_context &io_ctx): io_ctx(io_ctx) {}

    bool listen_on(std::string ifName);

    bool handleCanEvent(CanEvent &ce);

    void setServer(TCPServer *s) {
        this->server = s;
    }
    std::vector<std::string> getIfList();

private:
    TCPServer *server;
    std::map<std::string, CanDevice*> canDevices;

    boost::asio::io_context &io_ctx;

    std::function<bool(std::shared_ptr<ResocaMessage> rms)> sendPBMessage;
};


