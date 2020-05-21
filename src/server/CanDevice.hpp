//
// Created by xzvf on 2020. 05. 20..
//

#ifndef RESOCA_CANDEVICE_HPP
#define RESOCA_CANDEVICE_HPP

#include <string>
#include <utility>

#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "CanFrame.hpp"

class CanDevice {
public:
    CanDevice(std::string canIfName, boost::asio::io_context &ioContext) : canIfName(std::move(canIfName)),
    can_stream(NULL) {
        BOOST_LOG_TRIVIAL(trace) << "CONSTRUCTOR";
        can_stream = new boost::asio::posix::stream_descriptor(ioContext);
    }

    bool connect();

    void run();

    bool isConnected() { return connected; }

    std::string getCanIfName(){return canIfName;}

private:
    std::string canIfName;
    bool connected;
    volatile bool running = false;

    // used for internal interactions with the socketcan socket.
    int socket_fd;
    int canFDEnabled = 1;
    struct ifreq ifr;
    struct sockaddr_can addr;

    boost::asio::posix::stream_descriptor *can_stream;

    uint8_t canFrameBuffer[sizeof(canfd_frame)];

};




#endif //RESOCA_CANDEVICE_HPP
