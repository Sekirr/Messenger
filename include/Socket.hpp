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

class ServerSocket
{
private:
    Socket socketServer_;
    sockaddr_in serverAddress_{};
    int numberConnection_ = 5;

public:
    ServerSocket(Socket &&socket)
        : socketServer_(std::move(socket)) {};
    ~ServerSocket();
    void setNumberConnection(int num);
    void closeSocket();
    void createServerAddress(in_addr_t addr, int family, int port);
    bool bindSocket();
    bool listenSocket();
    bool acceptSocket();
};

class ClientSocket
{
private:
    Socket socketClient_;
    sockaddr_in clientAddress_{};

public:
    ClientSocket(Socket &&socket)
        : socketClient_(std::move(socket)) {};
    ~ClientSocket();
    void closeClientSocket();
    bool createClientAddress(const char *addr, int family, int port);
};
