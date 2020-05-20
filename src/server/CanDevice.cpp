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
    BOOST_LOG_TRIVIAL(trace) << "SOCKET FD: " << socket_fd;
    int soReuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &soReuse, sizeof(soReuse));

    // get interface index
    std::strcpy(ifr.ifr_name, canIfName.c_str());
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to get interface index for interface name: " << ifr.ifr_name
                                 << ". Does it exist? ; ifindex=" << ifr.ifr_ifindex;
        return false;

    }
    BOOST_LOG_TRIVIAL(trace) << "ifindex=" << ifr.ifr_ifindex;


    //bind to that socket.
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind to interface " << canIfName << " with ifindex="<<ifr.ifr_ifindex;
        return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "Assigning to can_stream";
    // create boost stream_descriptor for async io.
    can_stream->assign(socket_fd);

    BOOST_LOG_TRIVIAL(info) << "Connected successfully to can interface: " << canIfName;
    return true;
}

void CanDevice::run() {
    running = true;
    while(running) {

    }
}
