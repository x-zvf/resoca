#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <functional>

#include "CanDevice.hpp"
#include "../shared/protobuf/cpp/ResocaMessage.pb.h"
#include "../shared/CanFrame.hpp"
#include "../shared/CanEvent.hpp"

class CanDeviceManager {
public:
    CanDeviceManager(boost::asio::io_context &io_ctx): io_ctx(io_ctx) {}

    bool listen_on(std::string ifName);

    bool handlePBMessage(const ResocaMessage &msg);

    bool handleCanEvent(CanEvent &ce);

    void setSendPBMessage(std::function<bool(const ResocaMessage&)> func) {
        sendPBMessage = func;
    }

private:
    std::map<std::string, CanDevice*> canDevices;

    std::function<bool(const ResocaMessage&)> sendPBMessage;

    boost::asio::io_context &io_ctx;

};
