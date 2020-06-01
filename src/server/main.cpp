#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>

#include "CanDevice.hpp"
#include "../shared/CanFrame.hpp"

bool handleCanFrame(std::string ifName, CanFrame *canFrame){
    BOOST_LOG_TRIVIAL(debug) << "Handling can message from " << ifName <<": "<<canFrame->toString();
    delete canFrame;
    return true;
}


int main() {
    BOOST_LOG_TRIVIAL(trace) << "resoca starting" << std::endl;

    boost::asio::io_context ioContext;

    BOOST_LOG_TRIVIAL(trace) << "creating and connecting to can_devices" << std::endl;
    std::vector<CanDevice> canDevices;

    canDevices.emplace_back("vcan0", ioContext, handleCanFrame);

    for(CanDevice &canDevice : canDevices){
        BOOST_LOG_TRIVIAL(trace) << "letting can device connect";
        if(!canDevice.connect()){
            BOOST_LOG_TRIVIAL(error) << "CAN device failed to connect: " << canDevice.getCanIfName();
        }
        canDevice.run();
    }

    /*
    {
        CanFrame frame;
        frame.isCanFd = false;
        std::string dta = "ABCDEFGH";
        frame.copyData((const uint8_t *)dta.c_str(), dta.length());
        frame.canID = 0x1d;

        for(CanDevice &canDevice : canDevices){
            try {
canDevice.sendFrame(frame);
                BOOST_LOG_TRIVIAL(trace) << "send success";
            } catch (std::logic_error& e){

                BOOST_LOG_TRIVIAL(trace) << "send fail: " << e.what();
            }
        }
    }
    */


    BOOST_LOG_TRIVIAL(info) << "Initialized. running ioService.";
    ioContext.run();
    return 0;
}
