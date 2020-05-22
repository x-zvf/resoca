//
// Created by xzvf on 2020. 05. 22..
//

#include "CanEvent.hpp"

std::string CanEvent::toString() {
    std::stringstream ss;
    ss << "[ ";
    switch (eventType) {

        case IF_CONNECTED:
            ss << "IF_CONNECTED @ " << ifName << " ]: ";
            break;
        case IF_DISCONNECTED:
            ss << "IF_DISCONNECTED @ " << ifName << " ]: ";
            break;
        case FRAME_TX:
            ss << "FRAME_TX";
            ss << " @ " << ifName << " ]: ";
            break;
        case FRAME_RX:
            ss << "FRAME_RX";
            break;
    }
    return ss.str();
}
