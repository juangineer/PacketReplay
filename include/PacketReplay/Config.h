// Copyright 2023 juangineer
#pragma once
#include <cstdint>
#include <string>


struct Config
{
public:
    Config(int argc, char* argv[]);
    ~Config();

    std::string filename;
    std::string ipaddr;
    uint16_t port;
    bool isTcpClient;
    bool isTcpServer;
    bool isUDP;
};
