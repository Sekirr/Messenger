#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

bool checkSendMessage(ssize_t bytesSend, std::size_t &offset)
{
    if (bytesSend < 0)
    {
        std::cerr << "Failed to send message\n";
        return false;
    }
    if (bytesSend == 0)
    {
        return false;
    }

    offset += static_cast<std::size_t>(bytesSend);
    return true;
}

bool checkReceivedMessage(ssize_t bytesReceived, size_t &received)
{
    if (bytesReceived < 0)
    {
        std::cerr << "Failed to receive message\n";
        return false;
    }

    if (bytesReceived == 0)
    {
        std::cout << "Client closed the connection\n";
        return false;
    }

    received += static_cast<std::size_t>(bytesReceived);
}

int main()
{
    std::cout << "Client started\n";

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) != 1)
    {
        std::cerr << "Invalid server address\n";
        close(clientSocket);
        return 1;
    }

    if (connect(
            clientSocket,
            reinterpret_cast<sockaddr *>(&serverAddress),
            sizeof(serverAddress)) == -1)
    {
        std::cerr << "Server not connected\n";
        close(clientSocket);
        return 1;
    }

    const char *message = "Hello from client";

    bool connectionAlive = true;

    while (connectionAlive)
    {

        // send header message
        const uint32_t responseSend = static_cast<uint32_t>(std::strlen(message));
        const uint32_t networkResponseLength = htonl(responseSend);

        const std::size_t headerSize = std::strlen(message);
        std::size_t headerOffset = 0;

        while (headerOffset < headerSize)
        {
            ssize_t bytesSend = send(
                clientSocket,
                reinterpret_cast<const char *>(&networkResponseLength) + headerOffset,
                headerSize - headerOffset,
                0);

            if (!checkSendMessage(bytesSend, headerOffset))
            {
                connectionAlive = false;
                break;
            }
        }

        if (!connectionAlive)
        {
            break;
        }

        // send payload message
        const std::size_t totalSizeMessage = std::strlen(message);
        std::size_t offsetMessage = 0;

        while (offsetMessage < totalSizeMessage)
        {
            ssize_t bytesSend = send(
                clientSocket,
                reinterpret_cast<const char *>(&message) + offsetMessage,
                totalSizeMessage - offsetMessage,
                0);

            if (!(checkSendMessage(bytesSend, offsetMessage)))
            {
                connectionAlive = false;
                break;
            }
        }

        if (!connectionAlive)
        {
            break;
        }

        std::vector<char> buffer(totalSizeMessage);

        // response header
        uint32_t responseLength = 0;
        const std::size_t expected = sizeof(responseLength);
        std::size_t received = 0;

        while (received < expected)
        {
            ssize_t serverReceived = recv(
                clientSocket,
                reinterpret_cast<char *>(&responseLength) + received,
                expected - received,
                0

            );

            if (checkSendMessage(serverReceived, received))
            {
                connectionAlive = false;
                break;
            }
        }

        uint32_t
            // response payload
            while ()

                ssize_t serverReceived = recv(
                    clientSocket,
                    buffer.data(),
                    sizeof(buffer) - 1,
                    0);

        if (serverReceived > 0)
        {
            std::cout << "Recieved " << buffer << "\n";
        }
        std::cout << "Message sent\n";
    }
    return 0;
}
