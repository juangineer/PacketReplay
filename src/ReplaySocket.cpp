// Copyright 2023 juangineer
#include "PacketReplay/ReplaySocket.h"
#include <iostream>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ip = boost::asio::ip;

// Pointer to implementation
struct ReplaySocket::impl
{
    impl() : port(0), mode(UDP),
        tcpSocket(nullptr), udpSocket(nullptr)
    {
    }

    impl(const std::string& ipAddr, uint16_t port, ReplayMode mode) : port(port), mode(mode),
        tcpSocket(nullptr), udpSocket(nullptr)
    {
        ipaddr = ip::make_address_v4(ipAddr);
        switch (mode)
        {
        case TCP_CLIENT:
        case TCP_SERVER:
            tcpEndpoint = ip::tcp::endpoint(ipaddr, port);
            tcpSocket = new ip::tcp::socket(context);
            break;
        case UDP:
            udpEndpoint = ip::udp::endpoint(ipaddr, port);
            break;
        default:
            break;
        }
    }

    ~impl()
    {
        if (tcpSocket != nullptr)
        {
            tcpSocket->close();
            delete tcpSocket;
            tcpSocket = nullptr;
        }
        if (udpSocket != nullptr)
        {
            udpSocket->close();
            delete udpSocket;
            udpSocket = nullptr;
        }
    }

    void connect()
    {
        ip::tcp::acceptor acceptor(context, tcpEndpoint);
        switch (mode)
        {
        case TCP_CLIENT:
            std::cout << "Connecting to " << tcpEndpoint << std::endl;
            tcpSocket->connect(tcpEndpoint);
            std::cout << "CONNECTED" << std::endl;
            break;
        case TCP_SERVER:
            std::cout << "Listening on " << tcpEndpoint << std::endl;
            acceptor.accept(*tcpSocket);
            std::cout << "CONNECTED" << std::endl;
            break;
        case UDP:
            udpSocket = new ip::udp::socket(context, ip::udp::endpoint(ip::udp::v4(), 0));
            break;
        default:
            break;
        }
    }

    int sendPacket(const Packet& packet)
    {
        boost::system::error_code errorCode;
        switch (mode)
        {
        case TCP_CLIENT:
        case TCP_SERVER:
            boost::asio::write(*tcpSocket, boost::asio::buffer<uint8_t>(packet.bytes), errorCode);
            break;
        case UDP:
            udpSocket->send_to(boost::asio::buffer<uint8_t>(packet.bytes),
                udpEndpoint, 0, errorCode);
            break;
        default:
            break;
        }
        int err = errorCode.value();
        if (err != 0)
        {
            std::cout << "Socket Error: " << errorCode.message() << std::endl;
        }
        return err;
    }

    ip::address_v4 ipaddr;
    uint16_t port;
    ReplayMode mode;

    boost::asio::io_context context;
    ip::tcp::endpoint tcpEndpoint;
    ip::tcp::socket* tcpSocket;
    ip::udp::endpoint udpEndpoint;
    ip::udp::socket* udpSocket;
};

ReplaySocket::ReplaySocket(const std::string& ipAddr, uint16_t port, ReplayMode mode) : pImpl_(nullptr)
{
    pImpl_ = std::unique_ptr<impl>(new impl(ipAddr, port, mode));
}

ReplaySocket::~ReplaySocket()
{
}

void ReplaySocket::connect()
{
    if (pImpl_ != nullptr)
    {
        pImpl_->connect();
    }
}

int ReplaySocket::sendPacket(const Packet& packet)
{
    int err = -1;
    if (pImpl_ != nullptr)
    {
        err = pImpl_->sendPacket(packet);
    }

    return err;
}
