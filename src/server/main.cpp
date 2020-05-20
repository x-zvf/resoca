#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>

#include "CanDevice.hpp"

int main() {
    BOOST_LOG_TRIVIAL(trace) << "resoca starting" << std::endl;

    boost::asio::io_service ioService;

    BOOST_LOG_TRIVIAL(trace) << "creating and connecting to can_devices" << std::endl;
    std::vector<CanDevice> canDevices;

    CanDevice testCanDevice("vcan0", ioService);
    canDevices.emplace_back(testCanDevice);

    for(CanDevice &candevice : canDevices){
        candevice.connect();
    }

    ioService.run();
    return 0;
}
