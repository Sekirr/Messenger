#include "ClientManager.hpp"

void ClientManager::pushPoll(Socket socket)
{
    fds_.push_back({socket.getFd(),
                    POLLIN,
                    0});
}

std::error_code ClientManager::recvBuffer()
{
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

        ec = clientConnection.sendMessage(msgSendServer);

        if (ec)
        {
            return;
        }
    }
}

std::error_code ClientManager::startServer()
{
    while (!ec)
    {
        int ready = poll(fds_.data(), fds_.size(), -1);

        if (ready < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            if (errno == EBADF)
            {
                for (auto &item : fds_)
                {
                    if (item.revents == POLLNVAL)
                    {
                        item.fd = -1;
                    }
                }
            }
        }
        for (auto &item : fds_)
        {
            if (item.revents & POLLIN)
            {
                auto clientIt = clients_.find(item.fd);

                if (clientIt == clients_.end())
                {
                    continue;
                }
                ClientState &client = clientIt->second;

                client.
            }
        }
    }
}
