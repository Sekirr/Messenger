#pragma once
#include "Socket.hpp"
#include "Result.hpp"
#include <sys/socket.h>
#include <vector>
#include <cstdint>
#include <netinet/in.h>

class TCPConnection
{
private:
    Socket socket_;
    enum class State
    {
        Success,
        CloseConnect,
        Error
    };
    State state_ = State::Success;

public:
    TCPConnection(Socket &&socket) : socket_(std::move(socket)) {};
    TCPConnection(const TCPConnection &other) = delete;
    TCPConnection &operator=(const TCPConnection &other) = delete;
    TCPConnection(TCPConnection &&other) = default;
    TCPConnection &operator=(TCPConnection &&other) = default;
    Result<bool, std::error_code> sendAll(std::string buffer);
    Result<bool, std::error_code> recvExact();
};
