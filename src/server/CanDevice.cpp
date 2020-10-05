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
    auto event = CanEvent(canIfName, IF_CONNECTED);
    handleCanEvent(event);

    return true;
}

void CanDevice::run() {
    //BOOST_LOG_TRIVIAL(trace) << "Reading.";
    boost::asio::async_read(*can_stream, boost::asio::buffer(canFrameBuffer),
            boost::asio::transfer_at_least(sizeof(struct can_frame)),
            [this](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {

            if (errorCode) {
                BOOST_LOG_TRIVIAL(error) << "[ error @ " << canIfName << "]: " << errorCode;
                auto event = CanEvent(canIfName, IF_DISCONNECTED);
                handleCanEvent(event);
                reconnect();
                return;
            }


            if (bytesTransferred == sizeof(struct can_frame)) {
                struct can_frame frame = *reinterpret_cast<can_frame *>(canFrameBuffer);

                auto canFrame = new CanFrame(frame);
                BOOST_LOG_TRIVIAL(trace) << "[ RX @ " << canIfName << " ]: " << canFrame->toString();

                auto event = CanEvent(canIfName, FRAME_RX);
                event.canFrame = canFrame;
                handleCanEvent(event);

            } else if (bytesTransferred == sizeof(struct canfd_frame)) {

                struct canfd_frame frame = *reinterpret_cast<canfd_frame *>(canFrameBuffer);
                auto canFrame = new CanFrame(frame);
                BOOST_LOG_TRIVIAL(trace) << "[ RX @ " << canIfName << " ]: " << canFrame->toString();

                auto event = CanEvent(canIfName, FRAME_RX);
                event.canFrame = canFrame;
                handleCanEvent(event);

            } else {
                BOOST_LOG_TRIVIAL(error) << "Received unknown data with length="
                    << bytesTransferred;
            }

            run();
            });
}

void CanDevice::reconnect() {
    BOOST_LOG_TRIVIAL(trace) << "Attempting to reconnect.";
    auto reconnector = [this](const boost::system::error_code &errorCode) {
        if(errorCode) {
            BOOST_LOG_TRIVIAL(error) << "Encountered Error: " << errorCode;
        }
        _connected = false;
        can_stream->close();
        close(socket_fd);
        if (connect()) {
            BOOST_LOG_TRIVIAL(info) << "Reconnection successfull.";
            auto event = CanEvent(canIfName, IF_CONNECTED);
            handleCanEvent(event);
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

void CanDevice::sendFrame(const CanFrame &cf, int cbid) {
    BOOST_LOG_TRIVIAL(debug) << "SENDFRAME Attempting to write frame: " << cf.toString();
    auto frame = cf.asStruct();
    auto cfcb = new CanFrame(cf);
    writeFrame(frame, cf.getStructSize(), cbid);
    sentFrames[cbid] = cfcb;
    BOOST_LOG_TRIVIAL(debug) << "SENDFRAME DONE " << cf.toString();
    //std::free(frame);
}
void CanDevice::sendFrame(const struct canfd_frame &cf) {
    writeFrame(&cf, sizeof(cf), 0);
}

void CanDevice::sendFrame(const struct can_frame &cf) {
    writeFrame(&cf, sizeof(cf), 0);
}

void CanDevice::writeFrame(const void *frame, int len, int cbid) {
    boost::asio::async_write(*can_stream, boost::asio::buffer(frame, len),
            [this, cbid](boost::system::error_code errorCode, std::size_t len)
            {
                if(errorCode) {
                    BOOST_LOG_TRIVIAL(error) << "Encountered Error: " << errorCode;
                }
                BOOST_LOG_TRIVIAL(trace) << "Wrote frame with length: " << len;
                if(cbid) {

                    BOOST_LOG_TRIVIAL(trace) << "CALlING txCb";
                    txCb(cbid, (int)errorCode.value());
                }
            });
}

void CanDevice::txCb(int cbid, int error) {
    BOOST_LOG_TRIVIAL(trace) << "CB for CBID: " << cbid;
    if(!sentFrames.count(cbid)) {
        BOOST_LOG_TRIVIAL(error) << "Non-existant CBID: " << cbid;
        return;
    }
    auto cf = sentFrames[cbid];
    sentFrames.erase(cbid);

    CanEvent ce(canIfName, FRAME_TX);
    ce.err = error;
    ce.id = cbid;
    ce.sent = true;
    ce.canFrame = cf;
    BOOST_LOG_TRIVIAL(trace) << "SENDING CALLBACK EVENT: " << ce.toString();
    handleCanEvent(ce);
    BOOST_LOG_TRIVIAL(trace) << "POST txCb";
    ce.canFrame = nullptr; //to avoid double-free, because of cf.
}
