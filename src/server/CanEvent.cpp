//
// Created by xzvf on 2020. 05. 22..
//

#include "CanEvent.hpp"

std::string CanEvent::toString() {
    std::stringstream ss;
    ss << "[ {" << "ID: " << id << " ERR: " << err << "sent: " << sent << " } ";
    switch (eventType) {

        case IF_CONNECTED:
            ss << "IF_CONNECTED @ " << ifName << " ]: ";
            break;
        case IF_DISCONNECTED:
            ss << "IF_DISCONNECTED @ " << ifName << " ]: ";
            break;
        case FRAME_TX:
            ss << "FRAME_TX";
            ss << " @ " << ifName << " ]: " << canFrame->toString();
            break;
        case FRAME_RX:
            ss << "FRAME_RX";
            ss << " @ " << ifName << " ]: " << canFrame->toString();
            break;
    }
    return ss.str();
}
