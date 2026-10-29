#include "ClientManager.hpp"

void handleClient(TCPConnection clientConnection)
{
    std::error_code ec{};
    while (true)
    {
        std::vector<char> recvMessageServer;
        ec = clientConnection.recvMessage(recvMessageServer);

        if (ec)
        {
            return;
        }

        const std::string msgSendServer(recvMessageServer.begin(),
                                        recvMessageServer.end());

        std::cout << msgSendServer << std::endl;
        ec = clientConnection.sendMessage(msgSendServer);

        if (ec)
        {
            return;
        }
    }
}

ClientManager::~ClientManager()
{
    joinAll();
}

void ClientManager::stopAll()
{
    for (const auto &pair : managementThread)
    {
        shutdown(pair.first, SHUT_RDWR);
    }
}

void ClientManager::joinAll()
{
    for (auto &[fd, thread] : managementThread)
    {
        shutdown(fd, SHUT_RDWR);
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
