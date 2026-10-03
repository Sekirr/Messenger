#include "Socket.hpp"
#include <unistd.h>

Socket::Socket(int domain, int type, int protocol)
{
    fd_ = socket(domain, type, protocol);
}

Socket::Socket(Socket &&other) noexcept
{
    this->fd_ = other.fd_;
    if (other.fd_ != -1)
    {
        other.fd_ = -1;
    }
}
Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this != &other)
    {
        if (this->fd_ != -1)
        {
            close(this->fd_);
        }
        this->fd_ = other.fd_;
        other.fd_ = -1;
    }

    return *this;
}
Socket::~Socket()
{
    if (fd_ != -1)
    {
        close(fd_);
    }
}
int Socket::getFd() const noexcept
{
    return fd_;
}
