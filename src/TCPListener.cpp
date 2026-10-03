#include <TCPListener.hpp>

void TCPListener::createServerAddress(sa_family_t family, int port, in_addr_t address)
{
    serverAddress_.sin_family = family;
    serverAddress_.sin_port = htons(port);
    serverAddress_.sin_addr.s_addr = address;
}

bool TCPListener::createBind()
{
    if (statusOperation_ == State::Create)
    {
        if (bind(
                socket_.getFd(),
                reinterpret_cast<const sockaddr *>(&serverAddress_),
                sizeof(serverAddress_)) == -1)
        {
            std::cerr << "Error bind\n";
            return false;
        }
        statusOperation_ = State::Bind;
        return true;
    }
    return false;
};
bool TCPListener::listenServer()
{
    if (statusOperation_ == State::Bind)
    {
        if (listen(socket_.getFd(), 5) == -1)
        {
            std::cerr << "The server not listens\n";
            return false;
        }
        statusOperation_ = State::Listen;
        return true;
    }
    return false;
}
Result<Socket, std::error_code> TCPListener::acceptServer()
{
    if (statusOperation_ == State::Listen)
    {
        int acceptFd = accept(socket_.getFd(), nullptr, nullptr);
        if (acceptFd == -1)
        {
            std::error_code error(errno, std::generic_category());
            return Result<Socket, std::error_code>::failure(std::move(error));
        }
        Socket listenClient(acceptFd);
        return Result<Socket, std::error_code>::success(std::move(listenClient));
    }
    else
    {
        throw std::logic_error("Server not listen!");
    }
}
