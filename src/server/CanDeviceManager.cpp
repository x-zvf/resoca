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

std::vector<std::string> CanDeviceManager::getIfList() {
    std::vector<std::string> ifl;
    for(auto pair : canDevices) {
        ifl.push_back(pair.first);
    }
    return ifl;
}
