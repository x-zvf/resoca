#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>

#include "CanDevice.hpp"

int main() {
    BOOST_LOG_TRIVIAL(trace) << "resoca starting" << std::endl;

    boost::asio::io_context ioContext;

    BOOST_LOG_TRIVIAL(trace) << "creating and connecting to can_devices" << std::endl;
    std::vector<CanDevice> canDevices;

    canDevices.emplace_back("vcan0", ioContext);

    for(CanDevice &canDevice : canDevices){
        BOOST_LOG_TRIVIAL(trace) << "letting can device connect";
        if(!canDevice.connect()){
            BOOST_LOG_TRIVIAL(error) << "CAN device failed to connect: " << canDevice.getCanIfName();
        }
        canDevice.run();
    }

    BOOST_LOG_TRIVIAL(info) << "Initialized. running ioService.";
    ioContext.run();
    return 0;
}
