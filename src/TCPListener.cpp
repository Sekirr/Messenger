#include "TCPListener.hpp"

TCPListener::TCPListener(Socket &&socket)
    : socket_(std::move(socket)) {};

void TCPListener::createSockaddr(sa_family_t family, in_addr_t addr, in_port_t port)
{
    serverAddress_.sin_family = family;
    serverAddress_.sin_addr.s_addr = htonl(addr);
    serverAddress_.sin_port = htons(port);
};

void TCPListener::bind()
{
    if (::bind(
            socket_.getFd(),
            reinterpret_cast<const sockaddr *>(&serverAddress_),
            sizeof(serverAddress_)) < 0)
    {
        std::cerr << "Bind failed\n";
    }
};

void TCPListener::listen()
{
    if (::listen(
            socket_.getFd(),
            5) < 0)
    {
        std::cerr << "Listen failed\n";
    }
};

Socket TCPListener::accept()
{
    int result = ::accept(
        socket_.getFd(),
        nullptr,
        nullptr);
    if (result < 0)
    {
        std::cerr << "Accept failed\n";
        return Socket(-1);
    }
    return Socket(result);
}
