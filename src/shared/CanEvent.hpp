//
// Created by xzvf on 2020. 05. 22..
//

#ifndef RESOCA_CANEVENT_HPP
#define RESOCA_CANEVENT_HPP

#include "CanFrame.hpp"

enum CanEventType {
    IF_CONNECTED,
    IF_DISCONNECTED,

    FRAME_TX,
    FRAME_RX,
};

// Container object for every event
class CanEvent {
public:
    CanEvent(std::string ifName, CanEventType eventType) : ifName(ifName), eventType(eventType) {}

    ~CanEvent() {  free(canFrame); }

    CanEventType eventType;

    std::string ifName;

    CanFrame *canFrame = nullptr;

    std::string toString();

};


#endif //RESOCA_CANEVENT_HPP
