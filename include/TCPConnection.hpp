#pragma once

#include "Protocol.hpp"
#include "Socket.hpp"

#include <cstddef>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <system_error>
#include <vector>

class TCPConnection {
private:
  enum class State { Connected, Closed, Failed };

  Socket socket_;
  sockaddr_in clientAddress_{};
  State state_ = State::Connected;
  std::error_code lastError_;

  [[nodiscard]]
  std::error_code returnErrorState() const noexcept;

public:
  explicit TCPConnection(Socket &&socket) : socket_(std::move(socket)) {}

  TCPConnection(const TCPConnection &other) = delete;
  TCPConnection &operator=(const TCPConnection &other) = delete;

  TCPConnection(TCPConnection &&other) = default;
  TCPConnection &operator=(TCPConnection &&other) = default;

  std::error_code createClientSockAddr(const sa_family_t family, const int port,
                                       const std::string &address);
  [[nodiscard]]
  std::error_code connectTo();

  std::error_code sendAll(const void *dataSend, const std::size_t dataSize);
  std::error_code sendMessage(const std::string &message);

  std::error_code recvExact(void *dataRecv, const std::size_t sizeData);
  std::error_code recvMessage(std::vector<char> &buffer);
};
