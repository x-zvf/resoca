#include "config.h"
#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <exception>

#include "CanDevice.hpp"
#include "CanFrame.hpp"
#include "CanDeviceManager.hpp"
#include "TCPServer.hpp"


int main(int argc, char **argv) {
    boost::asio::io_context ioContext;

    std::vector<std::string> canIFs;
    std::string host = "0.0.0.0";
    int port = 23636;
    try {
        boost::program_options::options_description desc("server options");
        desc.add_options()
            ("help,h", "show help message")
            ("verbosity,v", boost::program_options::value<int>(), "set log level; highest: 0, default: 4")
            ("can,c", boost::program_options::value<std::vector<std::string>>(), "can device to listen on")
            ("host", boost::program_options::value<std::string>(),
             "Host on which to listen to; default: 0.0.0.0")
            ("port", boost::program_options::value<int>(),
             "Port on which to listen to; default: 23636");
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        if (vm.count("help")) {
            std::cout << "resoca-server v" << VERSION <<" \n" << desc << "\nWritten by Péter Bohner (xzvf), 2020\n";
            return 0;
        }

        int loglevel = 4;
        if (vm.count("verbosity")){
            loglevel = vm["verbosity"].as<int>();
        }
        boost::log::core::get()->set_filter (
            boost::log::trivial::severity >= loglevel
        );

        if(vm.count("can")) {
            canIFs = vm["can"].as<std::vector<std::string>>();
        } else {
            BOOST_LOG_TRIVIAL(fatal) << "No can interfaces specified";
            return 1;
        }

        if(vm.count("host")) {
            host = vm["host"].as<std::string>();
        }

        if(vm.count("port")) {
            port = vm["port"].as<int>();
        }
    }catch(std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse args: " << e.what();
        return 1;
    }

    BOOST_LOG_TRIVIAL(trace) << "creating and connecting to can_devices" << std::endl;

    auto canDeviceManager = new CanDeviceManager(ioContext);
    auto tcpServer = new TCPServer(host, port, ioContext);

    canDeviceManager->setServer(tcpServer);
    tcpServer->setCDM(canDeviceManager);

    for(auto interface : canIFs)
        canDeviceManager->listen_on(interface);
    tcpServer->listen();

    BOOST_LOG_TRIVIAL(info) << "Initialized. running ioService.";
    ioContext.run();
    return 0;
}
