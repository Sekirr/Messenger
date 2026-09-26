#include <iostream>
#include "TCPListener.hpp"

int main()
{
    Socket a(AF_INET, SOCK_STREAM, 0);
    TCPListener socketListen(std::move(a));
    socketListen.createSockaddr(
        AF_INET,
        INADDR_ANY,
        8080);
    socketListen.bind();
    socketListen.listen();
    Socket client = socketListen.accept();

    std::cout << client.getFd() << "\n";
}
