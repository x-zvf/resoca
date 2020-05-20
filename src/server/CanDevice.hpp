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

class CanDevice {
public:
    CanDevice(std::string canIfName, boost::asio::io_service &ioService) : canIfName(std::move(canIfName)),
    can_stream(NULL) {
        can_stream = new boost::asio::posix::stream_descriptor(ioService);
    }

    bool connect();

    void run();

    bool isConnected() { return connected; }

private:
    std::string canIfName;
    bool connected;
    volatile bool running = false;

    // used for internal interactions with the socketcan socket.
    int socket_fd;
    struct ifreq ifr;
    struct sockaddr_can addr;
    boost::asio::posix::stream_descriptor *can_stream;

};


#endif //RESOCA_CANDEVICE_HPP
