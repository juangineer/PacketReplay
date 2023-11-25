// Copyright 2023 juangineer
#include "PacketReplay/Config.h"
#include <iostream>
#include <iterator>
#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;


Config::Config(int argc, char* argv[]) :
    ipaddr("127.0.0.1"), port(8080),
    isTcpServer(false),
    isTcpClient(false),
    isUDP(false)
{
    try
    {
        po::options_description desc("cmd-line options");
        desc.add_options()
            ("filename,f", po::value<std::string>(&this->filename), "PCAP file")
            ("ipaddr,i", po::value<std::string>(&this->ipaddr), "IPv4 address")
            ("port,p", po::value<uint16_t>(&this->port), "Port")
            ("client,c", po::bool_switch(&this->isTcpClient)->default_value(false), "TCP client")
            ("server,s", po::bool_switch(&this->isTcpServer)->default_value(false), "TCP server")
            ("udp,u", po::bool_switch(&this->isUDP)->default_value(false), "UDP")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        // print out the config
        std::cout
            << "filename:    " << this->filename << "\n"
            << "ipaddr:      " << this->ipaddr << "\n"
            << "port:        " << this->port << "\n"
            << "isTcpClient: " << this->isTcpClient << "\n"
            << "isTcpServer: " << this->isTcpServer << "\n"
            << "isUDP:       " << this->isUDP << "\n"
            << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Exception of unknown type!\n";
    }
}

Config::~Config()
{
}
