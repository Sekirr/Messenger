#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <cstdio>

constexpr std::size_t MAX_LENGTH_MESSAGE = 64 * 1024;

bool checkMessageLength(const std::string &message)
{
    if (message.length() > MAX_LENGTH_MESSAGE)
    {
        std::cerr << "Error message. The length exceeds the limit. \n";
        return false;
    }
    if (message.empty())
    {
        std::cerr << "Error message. Length message 0 chars.\n";
        return false;
    }

    return true;
}

bool checkSend(ssize_t sendData)
{
    if (sendData == 0)
    {
        std::cerr << "Send 0 byte\n";
        return false;
    }
    if (sendData < 0)
    {
        std::cerr << "Error send\n";
        return false;
    }
    return true;
}

bool checkRecv(ssize_t recvData)
{
    if (recvData == 0)
    {
        std::cerr << "The other side has closed the connections\n";
        return false;
    }
    if (recvData < 0)
    {
        std::cerr << "Error recieved\n";
        return false;
    }
    return true;
}

bool sendAll(int socket, const char *buffer, std::size_t sizeBuffer)
{
    std::size_t offset = 0;

    while (offset < sizeBuffer)
    {
        const ssize_t sendData = send(
            socket,
            buffer + offset,
            sizeBuffer - offset,
            0);

        if (!checkSend(sendData))
        {
            return false;
        }

        offset += static_cast<std::size_t>(sendData);
    }

    return true;
}

bool recvAll(int socket, char *buffer, std::size_t sizeBuffer)
{
    std::size_t offset = 0;

    while (offset < sizeBuffer)
    {
        const ssize_t recvData = recv(
            socket,
            buffer + offset,
            sizeBuffer - offset,
            0);

        if (!checkRecv(recvData))
        {
            return false;
        }

        offset += static_cast<std::size_t>(recvData);
    }

    return true;
}

int main()
{
    std::cout << "Client started\n";

    // create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0)
    {
        std::cerr << "Failed to create socket client\n";
        return 1;
    }

    sockaddr_in serverAddress = {};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    int resultInetPton = inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (resultInetPton == 0)
    {
        std::cerr << "Is not a valid IP address\n";
        close(clientSocket);
        return 1;
    }
    if (resultInetPton < 0)
    {
        std::cerr << "System error inet_pton\n";
        close(clientSocket);
        return 1;
    }

    // connection
    int result = connect(
        clientSocket,
        reinterpret_cast<sockaddr *>(&serverAddress),
        sizeof(serverAddress));

    if (result < 0)
    {
        perror("connect");
        close(clientSocket);
        return 1;
        // std::cerr << "Connection from client to server error\n";
        // close(clientSocket);
        // return 1;
    }

    // sending
    std::string message;

    while (true)
    {
        if (!std::getline(std::cin, message))
        {
            break;
        }

        if (!checkMessageLength(message))
        {
            return 1;
        }

        // send header
        const uint32_t headerMessageSend = htonl(static_cast<uint32_t>(message.length()));
        constexpr std::size_t expectedHeaderSize = sizeof(headerMessageSend);

        if (!sendAll(
                clientSocket,
                reinterpret_cast<const char *>(&headerMessageSend),
                expectedHeaderSize))
        {
            break;
        }

        // send payload
        const std::size_t messageLength = message.length();

        if (!sendAll(
                clientSocket,
                message.data(),
                messageLength))
        {
            break;
        }

        // recv header

        uint32_t headerMessageRecv = 0;

        if (!recvAll(
                clientSocket,
                reinterpret_cast<char *>(&headerMessageRecv),
                expectedHeaderSize))
        {
            break;
        }

        const std::size_t messageLengthRecv = static_cast<std::size_t>(ntohl(headerMessageRecv));

        if (messageLengthRecv == 0 || messageLengthRecv > MAX_LENGTH_MESSAGE)
        {
            break;
        }
        std::vector<char> bufferMessage(messageLengthRecv);

        if (!recvAll(
                clientSocket,
                bufferMessage.data(),
                messageLengthRecv))
        {
            break;
        }

        std::cout << std::string(bufferMessage.data(), messageLengthRecv);
    }
    close(clientSocket);
}
