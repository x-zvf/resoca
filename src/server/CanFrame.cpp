//
// Created by xzvf on 2020. 05. 21..
//

#include <iomanip>
#include "CanFrame.hpp"

//struct canfd_frame {
//canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
//_u8    len;     /* frame payload length in byte (0 .. 64) */
//_u8    flags;   /* additional flags for CAN FD */
//_u8    __res0;  /* reserved / padding */
//_u8    __res1;  /* reserved / padding */
//_u8    data[64] __attribute__((aligned(8))); }

//struct can_frame {
//canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
//__u8    can_dlc; /* frame payload length in byte (0 .. 8) */
//__u8    __pad;   /* padding */
//__u8    __res0;  /* reserved / padding */
//__u8    __res1;  /* reserved / padding */
//__u8    data[8] __attribute__((aligned(8)));
//};
CanFrame::CanFrame(const struct can_frame &frame) {
    isCanFd = false;
    length = frame.can_dlc;
    setFlagsFromCanId(frame.can_id);
    copyData(frame.data, length);
}

CanFrame::CanFrame(const struct canfd_frame &frame) {
    isCanFd = true;
    length = frame.len;
    isCanFdBRS = frame.flags & CANFD_BRS;
    isCanFdESI = frame.flags & CANFD_ESI;
    setFlagsFromCanId(frame.can_id);
    copyData(frame.data, length);
}


void CanFrame::setFlagsFromCanId(uint32_t _canID) {
    isEFFFrame = _canID & CAN_EFF_FLAG;
    isRTRFrame = _canID & CAN_RTR_FLAG;
    isERRFrame = _canID & CAN_ERR_FLAG;

    //CAN_EFF_MASK == CAN_ERR_MASK; so we can just use one.
    if (isEFFFrame) {
        this->canID = _canID & CAN_EFF_MASK;
    } else {
        this->canID = _canID & CAN_SFF_MASK;
    }
}

CanFrame::~CanFrame() {
    free(data); // No need for NULL check here, free does that automatically.
}

void CanFrame::copyData(const uint8_t *const src, uint8_t len) {
    if (len > 64 || (len > 8 && (!isCanFd))) {
        throw std::invalid_argument("Data too long");
    }
    free(data);
    data = static_cast<uint8_t *>(malloc(len));
    if (data == nullptr) {
        throw std::bad_alloc();
    }
    memcpy(data, src, length);

}

std::string CanFrame::toString() const {
    std::stringstream ss;
    ss << (isCanFd ? "CANFD" : "CAN") << " frame: { "
       << (isRTRFrame ? "RTR " : "")
       << (isERRFrame ? "ERR " : "")
       << (isEFFFrame ? "EFF " : "SFF ")
       << (isCanFdESI ? "ESI " : "")
       << (isCanFdBRS ? "BRS " : "") << "}"
       << "[ " << std::setfill('0') << std::setw(2) << (int)length << " ] :";

    ss << "[";
    for (uint8_t i = 0; i < length; i++) {
        //ss << std::setfill('0') << std::setw(2) << std::hex << (int) data[i];
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int) data[i];
        if (i != length - 1) ss << ", ";
    }
    ss << "]";
    return ss.str();
}
