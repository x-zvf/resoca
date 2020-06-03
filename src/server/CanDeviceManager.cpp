#include "CanDeviceManager.hpp"
//#include <boost/bind.hpp>
#include <functional>

bool CanDeviceManager::listen_on(std::string ifName) {
    if(canDevices.count(ifName)) {
        BOOST_LOG_TRIVIAL(debug) << "Already listening on if: " << ifName;
        return true;
    }
    auto cd = new CanDevice(ifName, io_ctx, std::bind(&CanDeviceManager::handleCanEvent, this, std::placeholders::_1));
    canDevices[ifName] = cd;
    auto s = cd->connect();
    cd->run();
    return s;
}

bool CanDeviceManager::handleCanEvent(CanEvent &ce) {
    BOOST_LOG_TRIVIAL(debug) << "Handling CanEvent: " << ce.toString();
    return true;
}
bool CanDeviceManager::handlePBMessage(std::shared_ptr<ResocaMessage> rms) {
    BOOST_LOG_TRIVIAL(debug) << "Handling PBMessage in CanDevMgr: " << rms->DebugString();
}
