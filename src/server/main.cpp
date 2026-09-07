#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int main()
{
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

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket == -1)
    {
        std::cerr << "Failed to accept socket";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected\n";

    char buffer[1024]{};

    ssize_t bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0);

    if (bytesReceived == -1)
    {
        std::cerr << "Failed to receive message\n";
        close(clientSocket);
        close(serverSocket);
        return 1;
    }

    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        std::cout << "Received: " << buffer << "\n";
    }

    // sending a reply message
    ssize_t receivedSend;
    int offset = 0;
    const std::size_t totalSize = static_cast<std::size_t>(bytesReceived);

    while (offset != strlen(buffer))
    {
        receivedSend = send(
            clientSocket,
            buffer + offset,
            totalSize - offset,
            0);

        // error
        if (receivedSend < 0)
        {
            std::cout << "Response received error\n";
            close(clientSocket);
            break;
        }

        // did not send completely
        if (receivedSend > 0)
        {
            offset += receivedSend;
        }
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
