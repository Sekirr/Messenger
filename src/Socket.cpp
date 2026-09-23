#include "Socket.hpp"

// SOCKET
Socket::Socket(int domain, int type, int protocol)
{
    fd_ = socket(domain, type, protocol);
}

Socket::Socket(int fd)
{
    if (fd >= 0)
    {
        fd_ = fd;
    }
}

Socket::~Socket()
{
    if (fd_ >= 0)
    {
        close(fd_);
    }
}

int Socket::getFd() const
{
    return fd_;
}

Socket::Socket(Socket &&other) noexcept
{
    this->fd_ = other.fd_;
    other.fd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this != &other)
    {
        if (this->fd_ >= 0)
        {
            close(this->fd_);
        }
        this->fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

// SERVERSOCKET
ServerSocket::ServerSocket(Socket &&socket)
    : socketServer_(std::move(socket)) {};

ServerSocket::~ServerSocket()
{
    if (socketServer_.getFd() == -1)
    {
        close(socketServer_.getFd());
    }
}

void ServerSocket::setNumberConnection(int num)
{
    numberConnection_ = num;
}

void ServerSocket::closeSocket()
{
    close(socketServer_.getFd());
}

void ServerSocket::createServerAddress(in_addr_t addr, int family, int port)
{
    serverAddress_.sin_addr.s_addr = htonl(addr);
    serverAddress_.sin_family = family;
    serverAddress_.sin_port = htons(port);
}

bool ServerSocket::bindSocket()
{
    if (bind(
            socketServer_.getFd(),
            reinterpret_cast<const sockaddr *>(&serverAddress_),
            sizeof(serverAddress_)) == -1)
    {
        std::cerr << "Error create bind\n";
        return false;
    }
    return true;
}

bool ServerSocket::listenSocket()
{
    if (listen(socketServer_.getFd(), numberConnection_) == -1)
    {
        std::cerr << "Error listen server\n";
        return false;
    }
    return true;
}

bool ServerSocket::acceptSocket()
{
    if (accept(
            socketServer_.getFd(),
            nullptr,
            nullptr) == -1)
    {
        std::cerr << "The error accept socket\n";
        return false;
    }
    return true;
}

// CLIENTSOCKET
ClientSocket::ClientSocket(Socket &&socket)
    : socketClient_(std::move(socket)) {};

ClientSocket::~ClientSocket()
{
    if (socketClient_.getFd())
    {
        close(socketClient_.getFd());
    }
}

void ClientSocket::closeClientSocket()
{
    if (socketClient_.getFd() == -1)
    {
        close(socketClient_.getFd());
    }
}

bool ClientSocket::createClientAddress(const char *addr, int family, int port)
{
    clientAddress_.sin_family = family;
    clientAddress_.sin_port = htons(port);
    ssize_t result = inet_pton(
        family,
        addr,
        &clientAddress_.sin_addr);

    if (result == 0)
    {
        std::cerr << "The IP is not valid\n";
        return false;
    }
    if (result < 0)
    {
        std::cerr << "System error inet_pton\n";
        return false;
    }
    return true;
}
