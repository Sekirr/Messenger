#include "Socket.hpp"
#include <netinet/in.h>

class TCPListener
{
private:
    Socket socket_;
    sockaddr_in serverAddress_{};

public:
    TCPListener(Socket &&socket);
    TCPListener(const TCPListener &other) = delete;
    TCPListener &operator=(const TCPListener &other) = delete;
    TCPListener(TCPListener &&other) noexcept = default;
    TCPListener &operator=(TCPListener &&other) noexcept = default;

    void createSockaddr(sa_family_t family, in_addr_t addr, in_port_t port);
    void bind();
    void listen();
    Socket accept();
};
