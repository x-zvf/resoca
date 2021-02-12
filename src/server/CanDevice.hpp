//
// Created by xzvf on 2020. 05. 20..
//

#ifndef RESOCA_CANDEVICE_HPP
#define RESOCA_CANDEVICE_HPP

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <utility>
#include <functional>

#include "CanFrame.hpp"
#include "CanEvent.hpp"

class CanDevice {
   public:
    CanDevice(std::string canIfName, boost::asio::io_context &ioContext,
              std::function<bool(CanEvent&)> handleCanEvent)
        : canIfName(std::move(canIfName)),
          can_stream(nullptr),
          handleCanEvent(handleCanEvent),
          reconnectTimer(boost::asio::deadline_timer(
              ioContext, boost::posix_time::seconds(1))) {
        can_stream = new boost::asio::posix::stream_descriptor(ioContext);
    }

    bool connect();

    void sendFrame(const CanFrame &cf, int cbid);
    void sendFrame(const struct can_frame &cf);
    void sendFrame(const struct canfd_frame &cf);

    void run();

    void reconnect();

    bool isConnected() { return _connected; }

    std::string getCanIfName() { return canIfName; }

    bool isInterfaceUp();

   private:
    void writeFrame(const void *frame, int length, int cbid);

    std::string canIfName;
    bool _connected;

    // used for internal interactions with the socketcan socket.
    int socket_fd;
    int canFDEnabled = 1;
    struct ifreq ifr;
    struct sockaddr_can addr;

    boost::asio::posix::stream_descriptor *can_stream;

    uint8_t canFrameBuffer[sizeof(canfd_frame)];

    std::function<bool(CanEvent&)> handleCanEvent;

    boost::asio::deadline_timer reconnectTimer;

    void txCb(int cbid, int error);

    std::map<int,CanFrame *> sentFrames;
};

#endif  // RESOCA_CANDEVICE_HPP
