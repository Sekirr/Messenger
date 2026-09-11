#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

constexpr std::size_t MAX_LENGTH_MESSAGE = 64 * 1024;

bool checkMessageLength(const std::string &message)
{
    if (message.length() > MAX_LENGTH_MESSAGE)
    {
        std::cerr << "Error message. The length exceeds the limit. \n";
        return false;
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
        std::cerr << "Connection from client to server error\n";
        close(clientSocket);
        return 1;
    }

    // sending
    bool connectionAlive = true;
    std::string message;

    while (connectionAlive)
    {
        std::getline(std::cin, message);
        bool checkMessage = checkMessageLength(message);

        while (true)
        {
            if (!checkMessage)
            {
                std::cerr << "Invalid input. Message larger max size.\n";
                std::cout << "invalid input. Max message length 65424. You will try again?(y/n)";
                char answerFromClient;
                std::cin >> answerFromClient;
                if (answerFromClient == 'y')
                {
                    std::getline(std::cin, message);
                    bool checkMessage = checkMessageLength(message);
                    break;
                }
                else
                {
                    std::cout << "Goodbye\n";
                    close(clientSocket);
                    return 1;
                }
            }
        }
        // send header
        const uint32_t networkLengthMessageForHeader = htonl(static_cast<uint32_t>(message.length()));
        const std::size_t expected = sizeof(networkLengthMessageForHeader);
        std::size_t offset = 0;

        while (offset < expected)
        {
            ssize_t headerSend = send(
                clientSocket,
                reinterpret_cast<const char *>(&networkLengthMessageForHeader) + offset,
                expected - offset,
                0);

            if (headerSend < 0)
            {
                std::cerr << "Error send header\n";
                connectionAlive = false;
                break;
            }

            if (headerSend == 0)
            {
                std::cerr << "Sending header 0 byte\n";
                connectionAlive = false;
                break;
            }

            offset += static_cast<std::size_t>(headerSend);
        }

        if (!connectionAlive)
        {
            break;
        }

        // send payload
        offset = 0;

        while (offset < message.size())
        {
            ssize_t payloadSend = send(
                clientSocket,
                message.data() + offset,
                message.size() - offset,
                0);

            if (payloadSend < 0)
            {
                std::cerr << "Error send payload\n";
                connectionAlive = false;
                break;
            }
            if (payloadSend == 0)
            {
                std::cerr << "Sending payload 0 byte\n";
                connectionAlive = false;
                break;
            }

            offset += static_cast<std::size_t>(payloadSend);
        }
        if (!connectionAlive)
        {
            break;
        }

        // recieved from server
        uint32_t headerRecievedFromServer = 0;
        std::size_t sizeHeader = sizeof(headerRecievedFromServer);
        offset = 0;

        while (offset < sizeHeader)
        {
            ssize_t recvHeaderFromServer = recv(
                clientSocket,
                reinterpret_cast<char *>(&headerRecievedFromServer) + offset,
                sizeHeader - offset,
                0);

            if (recvHeaderFromServer == 0)
            {
                std::cerr << "The other party has completed the dispatch(header)\n";
                close(clientSocket);
                connectionAlive = false;
                return 1;
            }
            if (recvHeaderFromServer < 0)
            {
                std::cerr << "Error recieved from server(header)\n";
                close(clientSocket);
                connectionAlive = false;
                return 1;
            }

            offset += static_cast<std::size_t>(recvHeaderFromServer);
        }

        if (!connectionAlive)
        {
            close(clientSocket);
            break;
        }

        size_t sizeMessageFromServer = static_cast<std::size_t>(ntohl(headerRecievedFromServer));

        if (sizeMessageFromServer > MAX_LENGTH_MESSAGE)
        {
            std::cerr << "Recieved message size larger permissible value\n";
            close(clientSocket);
            return 1;
        }

        std::vector<char> bufferMessageFromServer(sizeMessageFromServer);
        offset = 0;
        while (offset < sizeMessageFromServer)
        {
            ssize_t recvFromServer = recv(
                clientSocket,
                bufferMessageFromServer.data() + offset,
                sizeMessageFromServer - offset,
                0);

            if (recvFromServer == 0)
            {
                std::cerr << "The other party has completed the dispatch\n";
                close(clientSocket);
                connectionAlive = false;
                return 1;
            }
            if (recvFromServer < 0)
            {
                std::cerr << "Error recieved from server\n";
                close(clientSocket);
                connectionAlive = false;
                return 1;
            }

            offset += static_cast<std::size_t>(recvFromServer);
        }
        if (!connectionAlive)
        {
            break;
        }

        std::cout << std::string(bufferMessageFromServer.data(), sizeMessageFromServer) << "\n";
    }
    close(clientSocket);
}
