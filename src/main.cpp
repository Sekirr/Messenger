#include <iostream>
#include "Socket.hpp"

int main()
{
    Socket a(AF_INET, SOCK_STREAM, 0);

    std::cout << "a before move: " << a.getFd() << '\n';

    Socket b(std::move(a));

    std::cout << "a after move: " << a.getFd() << '\n';
    std::cout << "b after move: " << b.getFd() << '\n';
}
