// Copyright 2023 juangineer
#pragma once
#include "PacketReplay/Packet.h"
#include <memory>
#include <string>
#include <boost/thread/concurrent_queues/sync_queue.hpp>

class PacketReader
{
public:
    explicit PacketReader(const std::string& filename);
    ~PacketReader();

    bool isValid();
    bool getNextPacket(Packet& packet);

private:
    PacketReader() = delete;

    // Pointer to implementation
    struct impl;
    std::unique_ptr<impl> pImpl_;
};
