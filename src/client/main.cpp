#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    ssize_t bytesSend = send(
        clientSocket,
        message,
        std::strlen(message),
        0);

    if (bytesSend == -1)
    {
        std::cerr << "Failed to send message\n";
        close(clientSocket);
        return 1;
    }

    char buffer[1024]{};
    ssize_t serverReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0);

    if (serverReceived > 0)
    {
        std::cout << "Recieved " << buffer << "\n";
    }
    std::cout << "Message sent\n";

    return 0;
}
