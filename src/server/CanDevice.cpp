//
// Created by xzvf on 2020. 05. 20..
//

#include "CanDevice.hpp"

bool CanDevice::connect() {
    //create socket.
    if ((socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to allocate socket for can device " << canIfName;
        return false;
    }
    //BOOST_LOG_TRIVIAL(trace) << "SOCKET FD: " << socket_fd;
    int soReuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &soReuse, sizeof(soReuse));
    setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canFDEnabled, sizeof(canFDEnabled));

    // get interface index
    std::strcpy(ifr.ifr_name, canIfName.c_str());
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to get interface index for interface name: " << ifr.ifr_name
                                 << ". Does it exist? ; ifindex=" << ifr.ifr_ifindex;
        return false;

    }
    //BOOST_LOG_TRIVIAL(trace) << "ifindex=" << ifr.ifr_ifindex;

    //bind to that socket.
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind to interface " << canIfName << " with ifindex=" << ifr.ifr_ifindex;
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "Assigning to can_stream";
    // create boost stream_descriptor for async io.
    can_stream->assign(socket_fd);

    if (!isInterfaceUp()) {
        BOOST_LOG_TRIVIAL(info) << "Interface not up: " << canIfName;
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "Connected successfully to can interface: " << canIfName;
    _connected = true;
    return true;
}

void CanDevice::run() {
    //BOOST_LOG_TRIVIAL(trace) << "Reading.";
    boost::asio::async_read(*can_stream, boost::asio::buffer(canFrameBuffer),
                            boost::asio::transfer_at_least(sizeof(struct can_frame)),
                            [this](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {

                                if (errorCode) {
                                    BOOST_LOG_TRIVIAL(error) << "[ error @ " << canIfName << "]: " << errorCode;
                                    reconnect();
                                    return;
                                }

                                CanFrame *canFrame;

                                if (bytesTransferred == sizeof(struct can_frame)) {
                                    struct can_frame frame = *reinterpret_cast<can_frame *>(canFrameBuffer);

                                    canFrame = new CanFrame(frame);

                                } else if (bytesTransferred == sizeof(struct canfd_frame)) {

                                    struct canfd_frame frame = *reinterpret_cast<canfd_frame *>(canFrameBuffer);

                                    canFrame = new CanFrame(frame);

                                } else {
                                    BOOST_LOG_TRIVIAL(error) << "Received unknown data with length="
                                                             << bytesTransferred;
                                }

                                BOOST_LOG_TRIVIAL(trace) << "[ RX @ " << canIfName << " ]: " << canFrame->toString();
                                handleCanFrame(canIfName, canFrame);
                                run();
                            });
}

void CanDevice::reconnect() {
    BOOST_LOG_TRIVIAL(trace) << "Attempting to reconnect.";
    auto reconnector = [this](const boost::system::error_code &errorCode) {
        _connected = false;
        can_stream->close();
        close(socket_fd);
        if (connect()) {
            BOOST_LOG_TRIVIAL(info) << "Reconnection successfull.";
            run();
        } else {
            BOOST_LOG_TRIVIAL(trace) << "Reconnection failed.";
            reconnect();
        }
    };
    reconnectTimer.expires_at(reconnectTimer.expires_at() + boost::posix_time::seconds(1));
    reconnectTimer.async_wait(reconnector);
}

bool CanDevice::isInterfaceUp() {
    if (ioctl(socket_fd, SIOCGIFFLAGS, &ifr) < 0) {
        BOOST_LOG_TRIVIAL(error) << "SIOCGIFLAGS failed.";
    }
    return !!(ifr.ifr_flags & IFF_UP);
}

void CanDevice::sendFrame(const CanFrame &cf) {
    BOOST_LOG_TRIVIAL(debug) << "Attempting to write frame: " << cf.toString();
    auto frame = cf.asStruct();
    writeFrame(frame, cf.getStructSize());
    std::free(frame);
}
void CanDevice::sendFrame(const struct canfd_frame &cf) {
    writeFrame(&cf, sizeof(cf));
}

void CanDevice::sendFrame(const struct can_frame &cf) {
    writeFrame(&cf, sizeof(cf));
}

void CanDevice::writeFrame(const void *frame, int len) {
    boost::asio::async_write(*can_stream, boost::asio::buffer(frame, len),
            [this](boost::system::error_code ec, std::size_t len)
        {
            BOOST_LOG_TRIVIAL(trace) << "Wrote frame with length: " << len;
        });
}
