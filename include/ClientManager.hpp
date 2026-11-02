#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <system_error>
#include "TCPConnection.hpp"
#include <poll.h>

struct ClientState
{
    Socket socket;

    std::vector<char> inputBuffer;
    std::vector<char> outputBuffer;

    std::size_t sendOffset = 0;
};

class ClientManager
{
private:
    std::vector<pollfd> fds_;
    std::unordered_map<int, ClientState> clients_;
    std::error_code ec{};

public:
    ClientManager(const ClientManager &other) = delete;
    ClientManager &operator=(const ClientManager &other) = delete;

    ClientManager(ClientManager &other) = default;
    ClientManager &operator=(ClientManager &other) = default;

    void pushPoll(Socket socket);
    std::error_code recvBuffer();
    std::error_code startServer();
};
