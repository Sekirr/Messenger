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
