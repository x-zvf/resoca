//
// Created by xzvf on 2020. 05. 21..
//

#ifndef RESOCA_CANFRAME_HPP
#define RESOCA_CANFRAME_HPP

#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <cstring>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <unitypes.h>

class CanFrame {
public:
    explicit CanFrame(const struct can_frame &frame);

    explicit CanFrame(const struct canfd_frame &frame);

    explicit CanFrame(){}

    ~CanFrame();

    int getStructSize() const;
    void* asStruct() const;


    uint32_t canID = 0;
    bool isEFFFrame = false;
    bool isRTRFrame = false; // remote transmission request
    bool isERRFrame = false; // error frame
    bool isCanFd = false;

    //canFD specific flags:
    // #define CANFD_BRS 0x01 /* bit rate switch (second bitrate for payload data) */
    // #define CANFD_ESI 0x02 /* error state indicator of the transmitting node */

    bool isCanFdESI = false;
    bool isCanFdBRS = false;


    void setFlagsFromCanId(uint32_t _canID);

    std::string toString() const;

    void copyData(const uint8_t *src, uint8_t len);

    uint8_t *data = nullptr;
    uint8_t length = 0;
};


#endif //RESOCA_CANFRAME_HPP
