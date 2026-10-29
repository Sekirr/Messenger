#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <system_error>
#include "TCPConnection.hpp"

class ClientManager
{
private:
    std::unordered_map<int, std::thread> managementThread{};
    std::error_code ec{};

public:
    ~ClientManager();
    ClientManager(const ClientManager &other) = delete;
    ClientManager &operator=(const ClientManager &other) = delete;

    void stopAll();
    void joinAll();
};
