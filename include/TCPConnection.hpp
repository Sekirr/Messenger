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
    std::error_code sendAll(const void *dataSend, std::size_t sizeSend);
    std::error_code sendMessage(std::string &message);
    std::error_code recvExact(void *dataRecv, std::size_t sizeData);
    std::error_code recvMessage(std::vector<char> *buffer);
};
