#pragma once
#include <iostream>
#include <sys/socket.h>

class Socket
{
private:
    int fd_;

public:
    explicit Socket(int fd)
        : fd_(fd) {}
    Socket(int domain, int type, int protocol);

    Socket(const Socket &other) = delete;
    Socket &operator=(const Socket &other) = delete;

    Socket(Socket &&other) noexcept;
    Socket &operator=(Socket &&other) noexcept;
    ~Socket();

    int getFd() const noexcept;
};
