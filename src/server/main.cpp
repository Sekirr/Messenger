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

    ssize_t receivedSend = send(
        clientSocket,
        buffer,
        std::strlen(buffer),
        0);

    if (receivedSend > 0)
    {
        std::cout << "Response sent\n";
    }
    else if (receivedSend == 0)
    {
        std::cout << "Stream the end\n";
    }
    else
    {
        std::cout << "Response received error";
    }
    close(clientSocket);
    close(serverSocket);
    return 0;
}
