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

    auto canDeviceManager = new CanDeviceManager(ioContext);

    auto tcpServer = new TCPServer("0.0.0.0", 23636, ioContext);

    canDeviceManager->setServer(tcpServer);
    tcpServer->setCDM(canDeviceManager);

    canDeviceManager->listen_on("vcan0");

    tcpServer->listen();

    BOOST_LOG_TRIVIAL(info) << "Initialized. running ioService.";
    ioContext.run();
    return 0;
}
