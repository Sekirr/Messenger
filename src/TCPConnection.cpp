#include "TCPConnection.hpp"

std::error_code TCPConnection::sendAll(const void *dataSend, std::size_t dataSize)
{
    const char *bytes = static_cast<const char *>(dataSend);

    if (state_ == State::Success)
    {
        std::size_t offset = 0;

        while (offset < dataSize)
        {
            ssize_t result = send(
                socket_.getFd(),
                bytes + offset,
                dataSize - offset,
                0);

            if (result == -1)
            {
                state_ = State::Error;
                std::error_code error(errno, std::generic_category());
                return error;
            }

            offset += static_cast<std::size_t>(result);
        }

    }
    return {};
}

std::error_code TCPConnection::sendMessage(std::string &message)
{
    if (state_ == State::Success)
    {
        uint16_t messageHeader = htons(message.size());
        std::error_code ec = sendAll(&messageHeader, sizeof(messageHeader));

        if (ec)
        {
            return ec;
        }

        ec = sendAll(message.data(), message.length());

        return ec;
    }
    return {};
}

std::error_code TCPConnection::recvExact(void *dataRecv, std::size_t(sizeData))
{
    if (state_ == State::Success)
    {
        std::size_t offset = 0;
        char *byte = static_cast<char *>(dataRecv);

        while (offset < sizeData)
        {
            ssize_t result = recv(
                socket_.getFd(),
                byte + offset,
                sizeData - offset,
                0);

            if (result == -1)
            {
                std::error_code error(errno, std::generic_category());
                state_ = State::Error;
                return error;
            }
            if (result == 0)
            {
                state_ = State::CloseConnect;
                return std::make_error_code(std::errc::not_connected)
            }

            offset += static_cast<std::size_t>(result);
        }
    }
    return {};
}

std::error_code TCPConnection::recvMessage(std::vector<char> *buffer)
{
    if (state_ == State::Success)
    {
        uint16_t messageHeader;

        std::error_code ec = recvExact(&messageHeader, sizeof(messageHeader));

        if (ec)
        {
            return ec;
        }

        std::size_t sizeBuffer = static_cast<std::size_t>(ntohl(messageHeader));
        ec = recvExact(buffer, sizeBuffer);

        if (ec)
        {
            return ec;
        }
    }
    return {};
}
