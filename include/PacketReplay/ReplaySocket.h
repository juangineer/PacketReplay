// Copyright 2023 juangineer
#pragma once
#include "PacketReplay/Packet.h"
#include <memory>
#include <string>

enum ReplayMode
{
    TCP_CLIENT,
    TCP_SERVER,
    UDP
};

class ReplaySocket
{
public:
    ReplaySocket(const std::string& ipAddr, uint16_t port, ReplayMode mode);
    ~ReplaySocket();

    void connect();
    int sendPacket(const Packet& packet);

private:
    ReplaySocket() = delete;

    // Pointer to implementation
    struct impl;
    std::unique_ptr<impl> pImpl_;
};
