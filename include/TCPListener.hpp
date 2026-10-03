#pragma once

#include "Socket.hpp"
#include "Result.hpp"
#include <netinet/in.h>
#include <variant>
#include <iostream>
#include <cerrno>

class TCPListener
{
private:
    enum class State
    {
        Create,
        Bind,
        Listen
    };

    Socket socket_;
    sockaddr_in serverAddress_{};

    State statusOperation_ = State::Create;

public:
    TCPListener(Socket socket)
        : socket_(std::move(socket)) {}

    TCPListener(TCPListener &&other) = default;
    TCPListener &operator=(TCPListener &&other) = default;

    void createServerAddress(sa_family_t family, int port, in_addr_t address);
    bool createBind();
    bool listenServer();
    [[nodiscard]]
    Result<Socket, std::error_code> acceptServer();
};
