#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int main()
{
    const std::size_t MAX_MESSAGE_SIZE = 64 * 1024;

    int serverSocket = createSocket();
    if (serverSocket == -1)
    {
        std::cout << "Failed to create socket\n";
        return 1;
    }
    std::cout << "Server started\n";

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(8080);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr *>(&serverAddress),
            sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind socket\n";
        close(serverSocket);

        return 1;
    }
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return 1;
    }

    // accept loop
    while (true)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket < 0)
        {
            std::cerr << "Failed to accept socket";
            close(serverSocket);
            return 1;
        }

        std::cout << "Client connected\n";

        bool connectionAlive = true;

        // client loop
        while (connectionAlive)
        {
            ssize_t bytesReceived;
            uint32_t networkLength = 0;
            std::size_t received = 0;
            const std::size_t expected = sizeof(networkLength);

            // header loop
            while (received < expected)
            {
                bytesReceived = recv(
                    clientSocket,
                    reinterpret_cast<char *>(&networkLength) + received,
                    expected - received,
                    0);

                // error
                if (bytesReceived < 0)
                {
                    std::cerr << "Failed to receive message\n";
                    connectionAlive = false;
                    break;
                }

                if (bytesReceived == 0)
                {
                    std::cout << "Client closed the connection\n";
                    connectionAlive = false;
                    break;
                }

                received += static_cast<std::size_t>(bytesReceived);
            }

            if (!connectionAlive)
            {
                break;
            }

            std::size_t payloadReceived = 0;
            const uint32_t messageLength = ntohl(networkLength);

            if (messageLength > MAX_MESSAGE_SIZE)
            {
                std::cerr << "Limit message exceeded\n";
                break;
            }
            std::vector<char> buffer(messageLength);

            // payload loop
            while (payloadReceived < messageLength)
            {
                bytesReceived = recv(
                    clientSocket,
                    buffer.data() + payloadReceived,
                    messageLength - payloadReceived,
                    0);

                if (bytesReceived < 0)
                {
                    std::cerr << "Failed to receive message\n";
                    connectionAlive = false;
                    break;
                }

                if (bytesReceived == 0)
                {
                    std::cout << "Client closed the connection\n";
                    connectionAlive = false;
                    break;
                }

                payloadReceived += static_cast<std::size_t>(bytesReceived);
            }

            if (!connectionAlive)
            {
                break;
            }

            // send header
            const uint32_t responseLength = static_cast<uint32_t>(buffer.size());
            const uint32_t networkResponseLength = htonl(responseLength);
            std::size_t headerOffset = 0;
            const std::size_t headerSize = sizeof(networkResponseLength);

            while (headerOffset < headerSize)
            {
                const ssize_t bytesSentHeader = send(
                    clientSocket,
                    reinterpret_cast<const char *>(&networkResponseLength) + headerOffset,
                    headerSize - headerOffset,
                    0);

                // error
                if (bytesSentHeader < 0)
                {
                    connectionAlive = false;
                    std::cerr << "Failed to send header response";
                    break;
                }
                if (bytesSentHeader == 0)
                {
                    connectionAlive = false;
                    break;
                }

                headerOffset += static_cast<std::size_t>(bytesSentHeader);
            }

            if (!connectionAlive)
            {
                break;
            }

            // send payload
            const std::size_t totalSize = buffer.size();
            std::size_t offset = 0;

            while (offset < totalSize)
            {
                const ssize_t bytesSent = send(
                    clientSocket,
                    buffer.data() + offset,
                    totalSize - offset,
                    0);

                // error
                if (bytesSent < 0)
                {
                    connectionAlive = false;
                    std::cerr << "Failed to send response\n";
                    break;
                }

                if (bytesSent == 0)
                {
                    connectionAlive = false;
                    break;
                }

                // advance by the number of bytes sent
                offset += static_cast<std::size_t>(bytesSent);
            }
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
