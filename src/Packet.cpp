// Copyright 2023 juangineer
#include "PacketReplay/Packet.h"

using boost::chrono::nanoseconds;

Packet::Packet() : timestamp(0)
{
}

Packet::Packet(nanoseconds ns, const uint8_t* payload, size_t size) : timestamp(ns)
{
    bytes = std::vector<uint8_t>(payload, payload + size);
}

Packet::~Packet()
{
}
