#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "CanDevice.hpp"
#include "../shared/CanFrame.hpp"
#include "CanDeviceManager.hpp"
#include "TCPServer.hpp"


int main() {
    BOOST_LOG_TRIVIAL(trace) << "resoca starting" << std::endl;

    boost::asio::io_context ioContext;

    BOOST_LOG_TRIVIAL(trace) << "creating and connecting to can_devices" << std::endl;

    CanDeviceManager canDeviceManager(ioContext);

    canDeviceManager.listen_on("vcan0");

    TCPServer tcpServer("0.0.0.0", 23636, ioContext);


    //plumb them up
    tcpServer.setHandleReceivePBMessage(std::bind(&CanDeviceManager::handlePBMessage, std::ref(canDeviceManager), std::placeholders::_1));
    canDeviceManager.setSendPBMessage(std::bind(&TCPServer::sendPBMessage, std::ref(tcpServer), std::placeholders::_1));

    tcpServer.listen();

    BOOST_LOG_TRIVIAL(info) << "Initialized. running ioService.";
    ioContext.run();
    return 0;
}
