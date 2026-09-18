#include "Socket.cpp"

// SOCKET
Socket::Socket(int domain, int type, int protocol)
{
    fd_ = socket(domain, type, protocol);
}

Socket::~Socket()
{
    close(fd_);
}

int Socket::getFd()
{
    return fd_;
}

Socket::Socket(Socket &&other)
{
    this->fd_ = other.fd_;
    other.fd_ = -1;
}

Socket &Socket::operator=(Socket &&other)
{
    delete this;
    this->fd_ = other.fd_;
    other.fd_ = -1;
    return *this;
}

// SERVERSOCKET
ServerSocket::ServerSocket(Socket &&socket)
    : socketServer_(std::move(socket)) {};

ServerSocket::~ServerSocket()
{
    close(socketServer_.getFd());
}

void ServerSocket::setNumberConnection(int num)
{
    numberConnection_ = num;
}

void ServerSocket::closeSocket()
{
    close(socketServer_.getFd());
}

void ServerSocket::createServerAddress(in_addr_t addr, int family, int port)
{
    serverAddress_.sin_addr.s_addr = htonl(addr);
    serverAddress_.sin_family = family;
    serverAddress_.sin_port = htons(port);
}

bool ServerSocket::bindSocket()
{
    if (bind(
            socketServer_.getFd(),
            reinterpret_cast<const sockaddr *>(&serverAddress_),
            sizeof(serverAddress_)) == -1)
    {
        std::cerr << "Error create bind\n";
        return false;
    }
    return true;
}

bool ServerSocket::listenSocket()
{
    if (listen(socketServer_.getFd(), numberConnection_) == -1)
    {
        std::cerr << "Error listen server\n";
        return false;
    }
    return true;
}

// CLIENTSOCKET
ClientSocket::ClientSocket(Socket &&socket)
    : socketClient_(std::move(socket)) {};

ClientSocket::~ClientSocket()
{
    close(socketClient_.getFd());
}

void ClientSocket::closeClientSocket()
{
    close(socketClient_.getFd());
}
