#include "TCPConnection.hpp"

Result<bool, std::error_code> TCPConnection::sendAll(std::string buffer)
{
    if (state_ == State::Success)
    {
        // sends header
        uint16_t messageHeader = htons(buffer.size());
        std::size_t sizeMessageHeader = sizeof(messageHeader);
        std::size_t offset = 0;
        while (offset < sizeMessageHeader)
        {
            ssize_t sendHeader = send(
                socket_.getFd(),
                reinterpret_cast<const char *>(messageHeader) + offset,
                sizeMessageHeader - offset,
                0);

            if (sendHeader == -1)
            {
                std::error_code error(errno, std::generic_category());
                return Result<bool, std::error_code>::failure(error);
            }
            offset += static_cast<std::size_t>(sendHeader);
        }

        // sends message
        ssize_t sendByte;
        std::size_t sizeMessage = buffer.size();
        offset = 0;

        while (offset < sizeMessage)
        {
            sendByte = send(
                socket_.getFd(),
                buffer.data() + offset,
                sizeMessage - offset,
                0);

            if (sendByte == -1)
            {
                state_ = State::Error;
                std::error_code error(errno, std::generic_category());
                return Result<bool, std::error_code>::failure(error);
            }
            offset += static_cast<std::size_t>(sendByte);
        }

        return Result<bool, std::error_code>::returnValue(true);
    }
    return Result<bool, std::error_code>::returnValue(false);
}

Result<bool, std::error_code> TCPConnection::recvExact()
{
    if (state_ == State::Success)
    {
        // receive header
        uint16_t messageHeader;
        std::size_t sizeMessageHeader = sizeof(messageHeader);
        std::size_t offset = 0;

        while (offset < sizeMessageHeader and state_ == State::Success)
        {
            ssize_t recvHeader = recv(
                socket_.getFd(),
                reinterpret_cast<char *>(messageHeader) + offset,
                sizeMessageHeader - offset,
                0);

            if (recvHeader == -1)
            {
                std::error_code error(errno, std::generic_category());
                state_ = State::Error;
                return Result<bool, std::error_code>::failure(error);
            }

            // connection close()
            if (recvHeader == 0)
            {
                socket_.~Socket();
                state_ = State::CloseConnect;
                break;
            }

            offset += static_cast<std::size_t>(recvHeader);
        }

        // receive message
        offset = 0;
        std::string recvMessage;
        std::size_t lengthMessage = static_cast<std::size_t>(ntohs(messageHeader));

        while (offset < lengthMessage and state_ == State::Success)
        {
            ssize_t recvMessage = recv(
                socket_.getFd(),
                &recvMessage - offset,
                lengthMessage + offset,
                0);

            if (recvMessage == -1)
            {
                std::error_code error(errno, std::generic_category());
                state_ = State::Error;
                return Result<bool, std::error_code>::failure(error);
            }

            // connection close()
            if (recvMessage == 0)
            {
                socket_.~Socket();
                state_ = State::CloseConnect;
                break;
            }

            offset += static_cast<std::size_t>(recvMessage);
        }
    }
    return Result<bool, std::error_code>::returnValue(false);
}
