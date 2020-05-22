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

#include "../shared/CanFrame.hpp"

class CanDevice {
public:
    CanDevice(std::string canIfName, boost::asio::io_context &ioContext, bool (*handleCanFrame)(std::string ifname, CanFrame *cf))
            : ioContext(ioContext),
              canIfName(std::move(canIfName)),
              can_stream(nullptr),
              handleCanFrame(handleCanFrame),
              reconnectTimer(boost::asio::deadline_timer(ioContext, boost::posix_time::seconds(1))) {
        can_stream = new boost::asio::posix::stream_descriptor(ioContext);
    }

    bool connect();

    void run();

    void reconnect();


    bool isConnected() { return _connected; }

    std::string getCanIfName() { return canIfName; }

    bool isInterfaceUp();

private:

    boost::asio::io_context &ioContext;

    std::string canIfName;
    bool _connected;
    volatile bool running = false;

    // used for internal interactions with the socketcan socket.
    int socket_fd;
    int canFDEnabled = 1;
    struct ifreq ifr;
    struct sockaddr_can addr;

    boost::asio::posix::stream_descriptor *can_stream;

    uint8_t canFrameBuffer[sizeof(canfd_frame)];

    boost::asio::deadline_timer reconnectTimer;

    bool (*handleCanFrame)(std::string ifname, CanFrame *cf);

};


#endif //RESOCA_CANDEVICE_HPP
