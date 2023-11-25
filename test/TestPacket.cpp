// Copyright 2023 juangineer
#include "PacketReplay/Packet.h"
#include <iostream>
#include <boost/chrono/chrono.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestPacket)

static boost::chrono::nanoseconds timestamp(100);
static uint8_t payload[] = { 1, 2, 3, 4, 5 };
static size_t size = 5;

BOOST_AUTO_TEST_CASE(TestPacketConstructor)
{
    std::cout << "TestPacketConstructor" << std::endl;
    Packet p(timestamp, payload, size);
    BOOST_CHECK_EQUAL(p.bytes.size(), size);
    for (int i = 0; i < size; ++i)
    {
        BOOST_CHECK_EQUAL(p.bytes[i], payload[i]);
    }
}

BOOST_AUTO_TEST_CASE(TestPacketAssignment)
{
    std::cout << "TestPacketAssignment" << std::endl;
    Packet p1;
    Packet p2(timestamp, payload, size);
    p1 = p2;
    BOOST_CHECK_EQUAL(p1.timestamp.count(), p2.timestamp.count());
    BOOST_CHECK_EQUAL_COLLECTIONS(p1.bytes.begin(), p1.bytes.end(), p2.bytes.begin(), p2.bytes.end());
}

BOOST_AUTO_TEST_SUITE_END()
