#include "Socket.hpp"
#include <netinet/in.h>
#include <variant>

// t - type success value
// e - type error value
template <typename T, typename E>
class Result
{
private:
    std::variant<T, E> data_;
};

class TCPListener
{
private:
    Socket socket_;
    sockaddr_in serverAddress_{};
    enum class State
    {
        Created,
        Bound,
        Listening
    };
    State state_ = State::Created;
    enum class AcceptError
    {
    };
    std::variant<Socket, AcceptError> result;

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
