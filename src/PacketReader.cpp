// Copyright 2023 juangineer
#include "PacketReplay/PacketReader.h"
#include <cstdint>
#include <ctime>
#include <iostream>
#include <boost/chrono/chrono.hpp>
#include <boost/filesystem.hpp>
#include <Layer.h>
#include <Packet.h>
#include <PcapFileDevice.h>
#include <PcapPlusPlusVersion.h>
#include <RawPacket.h>
#include <SystemUtils.h>
#include <TcpLayer.h>
#include <UdpLayer.h>

using boost::filesystem::path;
using boost::chrono::seconds;
using boost::chrono::nanoseconds;


// Pointer to implementation
struct PacketReader::impl
{
    impl() : isValid(false), reader(nullptr), count(0)
    {
    }

    explicit impl(const std::string& filename) : isValid(false), reader(nullptr), count(0)
    {
        // check if file exists
        path filepath = path(filename);
        path canonicalPath = boost::filesystem::canonical(filepath.parent_path(), boost::filesystem::current_path());
        isValid = boost::filesystem::exists(filepath);

        if (isValid)
        {
            std::cout << "Using PcapPlusPlus " << pcpp::getPcapPlusPlusVersionFull() << "\n" << std::endl;

            // open a pcap/pcapng file for reading
            reader = pcpp::IFileReaderDevice::getReader(filename);

            if (!reader->open())
            {
                isValid = false;
                std::cerr << "Unable to open " << filepath.filename() << " in " << canonicalPath << std::endl;
            }
        }
        else
        {
            std::cerr << "Unable to find " << filepath.filename() << " in " << canonicalPath << std::endl;
        }
    }

    ~impl()
    {
        if (reader != nullptr)
        {
            // close the file
            reader->close();

            // free reader memory
            delete reader;
            reader = nullptr;
        }
    }

    bool getNextPacket(Packet& packet)
    {
        // read packets from the file
        pcpp::RawPacket rawPacket;
        bool success = reader->getNextPacket(rawPacket);

        if (success)
        {
            // parse the raw packet into a parsed packet
            timespec ts = rawPacket.getPacketTimeStamp();
            nanoseconds ns = seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec);
            pcpp::Packet parsedPacket(&rawPacket);

            pcpp::Layer* layer = nullptr;
            pcpp::Layer* tcpLayer = parsedPacket.getLayerOfType<pcpp::TcpLayer>();
            if (tcpLayer != nullptr)
            {
                layer = tcpLayer;
                //std::cout << "TCP layer found with " << tcpLayer->getLayerPayloadSize() << " bytes" << std::endl;
            }

            pcpp::Layer* udpLayer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
            if (udpLayer != nullptr)
            {
                layer = udpLayer;
                //std::cout << "UDP layer found with " << udpLayer->getLayerPayloadSize() << " bytes" << std::endl;
            }

            if (layer != nullptr)
            {
                Packet p(ns, layer->getLayerPayload(), layer->getLayerPayloadSize());
                packet = p;
                ++count;
            }
        }

        return success;
    }

    uint32_t count;
    bool isValid;
    pcpp::IFileReaderDevice* reader;
};


PacketReader::PacketReader(const std::string& filename) : pImpl_(nullptr)
{
    pImpl_ = std::unique_ptr<impl>(new impl(filename));
}

PacketReader::~PacketReader()
{
}

bool PacketReader::isValid()
{
    bool valid = false;
    if (pImpl_ != nullptr)
    {
        valid = pImpl_->isValid;
    }
    return valid;
}

bool PacketReader::getNextPacket(Packet& packet)
{
    bool success = false;
    if (pImpl_ != nullptr)
    {
        success = pImpl_->getNextPacket(packet);
    }
    return success;
}
