// Copyright 2023 juangineer
#pragma once
#include <cstdint>
#include <ctime>
#include <vector>
#include <boost/chrono/chrono.hpp>

struct Packet
{
    Packet();
    Packet(boost::chrono::nanoseconds ns, const uint8_t* payload, size_t size);
    ~Packet();

    boost::chrono::nanoseconds timestamp;
    std::vector<uint8_t> bytes;
};
