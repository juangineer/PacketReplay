// Copyright 2023 juangineer
#include "PacketReplay/Config.h"
#include "PacketReplay/PacketReader.h"
#include "PacketReplay/ReplaySocket.h"
#include "version.h"
#include <iostream>
#include <boost/chrono/chrono.hpp>
#include <boost/thread/concurrent_queues/sync_queue.hpp>
#include <boost/thread/thread.hpp>

using boost::concurrent::sync_queue;
using boost::chrono::nanoseconds;


void readPackets(PacketReader* pr, sync_queue<Packet>* packetQ)
{
    uint32_t count = 0;
    Packet p;
    std::cout << "Reading packets\n" << std::endl;
    while (pr->getNextPacket(p))
    {
        packetQ->push(p);
        ++count;
    }
    packetQ->close();
    std::cout << "Read " << count << " packets\n" << std::endl;
}

void sendPacket(ReplaySocket* rs, sync_queue<Packet>* packetQ)
{
    uint32_t count = 0;
    nanoseconds lastTimestamp(0);
    nanoseconds delay(100000);
    Packet p;
    std::cout << "Sending packets\n" << std::endl;
    while (!packetQ->empty() || !packetQ->closed())
    {
        // dequeue
        p = packetQ->pull();

        // update timestamp
        if (lastTimestamp.count() > 0)
        {
            delay = p.timestamp - lastTimestamp;
        }
        lastTimestamp = p.timestamp;

        // delay the send
        if (delay.count() > 0)
        {
            boost::this_thread::sleep_for(delay);
        }

        // now send the packet
        int err = rs->sendPacket(p);
        if (err == 0)
        {
            std::cout << "   Sent " << p.bytes.size() << " bytes\n";
            ++count;
        }
        else
        {
            std::cerr << "Socket Error #" << err << std::endl;
            break;
        }
    }
    std::cout << "Sent " << count << " packets\n" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "PacketReplay v" << VERSION << "\n" << std::endl;

    // read config
    Config config = Config(argc, argv);
    if (config.filename.empty())
    {
        std::cerr << "ERROR - filename not specified" << std::endl;
        return -1;
    }

    // get the packet reader
    PacketReader pr(config.filename);
    if (!pr.isValid())
    {
        std::cerr << "ERROR - unable to read " << config.filename << std::endl;
        return -1;
    }

    // create socket
    ReplayMode mode(UDP);
    if (config.isTcpClient)
    {
        mode = TCP_CLIENT;
        std::cout << "TCP client mode" << std::endl;
    }
    else if (config.isTcpServer)
    {
        mode = TCP_SERVER;
        std::cout << "TCP server mode" << std::endl;
    }
    else
    {
        std::cout << "UDP sender mode" << std::endl;
    }

    ReplaySocket rs(config.ipaddr, config.port, mode);
    rs.connect();

    // start read/write
    sync_queue<Packet> packetQ;
    boost::thread readThread(&readPackets, &pr, &packetQ);
    boost::thread sendThread(&sendPacket, &rs, &packetQ);
    readThread.join();
    sendThread.join();

    std::cout << packetQ.size() << std::endl;

    return 0;
}
