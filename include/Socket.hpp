#pragma once
#include <iostream>
#include <sys/socket.h> // socket, bind, listen, accept, connect, send, recv
#include <netinet/in.h> // sockaddr_in, htons, htonl, ntohs, ntohl
#include <arpa/inet.h>  // inet_pton, inet_ntop
#include <unistd.h>     // close
#include <utility>

class Socket
{
private:
    int fd_ = -1;

public:
    Socket(int domain, int type, int protocol);
    explicit Socket(int fd);
    ~Socket();
    int getFd() const;

    Socket(Socket &&other) noexcept;
    Socket &operator=(Socket &&other) noexcept;

    Socket(const Socket &other) = delete;
    Socket &operator=(const Socket &other) = delete;
};
