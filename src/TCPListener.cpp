#include "TCPListener.hpp"

TCPListener::TCPListener(Socket &&socket)
    : socket_(std::move(socket)) {};

void TCPListener::createSockaddr(sa_family_t family, in_addr_t addr, in_port_t port)
{
    if (state_ == State::Created)
    {
        serverAddress_.sin_family = family;
        serverAddress_.sin_addr.s_addr = htonl(addr);
        serverAddress_.sin_port = htons(port);
    }
};

void TCPListener::bind()
{
    if (state_ == State::Created)
    {
        if (::bind(
                socket_.getFd(),
                reinterpret_cast<const sockaddr *>(&serverAddress_),
                sizeof(serverAddress_)) < 0)
        {
            std::cerr << "Bind failed\n";
        }
        else
        {
            state_ = State::Bound;
        }
    }
};

void TCPListener::listen()
{
    if (state_ == State::Bound)
    {
        if (::listen(
                socket_.getFd(),
                5) < 0)
        {
            std::cerr << "Listen failed\n";
        }
        else
        {
            state_ = State::Listening;
        }
    }
    else
    {
        std::cerr << "Not bind server\n";
    }
};

Socket TCPListener::accept()
{
    if (state_ == State::Listening)
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
    else
    {
        std::cerr << "Server not listening\n";
        return Socket(-1);
    }
}
