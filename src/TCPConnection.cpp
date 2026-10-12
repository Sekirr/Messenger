#include "TCPConnection.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <netinet/in.h>
#include <sys/socket.h>

std::error_code
TCPConnection::createClientSockAddr(const sa_family_t family, const int port,
                                    const std::string &address) {
  clientAddress_.sin_family = family;
  clientAddress_.sin_port = htons(port);

  const int result =
      inet_pton(family, address.c_str(), &clientAddress_.sin_addr.s_addr);

  if (result == 0) {
    lastError_ = std::make_error_code(std::errc::address_family_not_supported);
    socket_.reset();
    state_ = State::Failed;
    return lastError_;
  }

  if (result == -1) {
    lastError_ = {errno, std::generic_category()};
    socket_.reset();
    state_ = State::Failed;
    return lastError_;
  }

  return {};
}

std::error_code TCPConnection::connectTo() {
  if (state_ == State::Connected) {
    const int result = connect(
        socket_.getFd(), reinterpret_cast<const sockaddr *>(&clientAddress_),
        sizeof(clientAddress_));

    if (result == -1) {
      int error = errno;
      lastError_ = {error, std::generic_category()};
      socket_.reset();
      state_ = State::Failed;
      return lastError_;
    }
  }

  return returnErrorState();
}

std::error_code TCPConnection::sendAll(const void *dataSend,
                                       const std::size_t dataSize) {
  const auto *bytes = static_cast<const char *>(dataSend);

  if (state_ == State::Connected) {
    std::size_t offset = 0;

    while (offset < dataSize) {
      const ssize_t result =
          send(socket_.getFd(), bytes + offset, dataSize - offset, 0);

      if (result == -1) {
        int error = errno;

        if (error == EINTR) {
          continue;
        }

        lastError_ = {error, std::generic_category()};
        socket_.reset();
        state_ = State::Failed;

        return lastError_;
      }

      offset += static_cast<std::size_t>(result);
    }

    return {};
  }

  return returnErrorState();
}

std::error_code TCPConnection::sendMessage(const std::string &message) {
  if (state_ == State::Connected) {

    if (message.size() > protocol::MaxMessageSize) {
      lastError_ = std::make_error_code(std::errc::message_size);
      socket_.reset();
      state_ = State::Failed;
      return lastError_;
    }
    const uint32_t messageHeader = htonl(message.size());

    std::error_code ec = sendAll(&messageHeader, sizeof(messageHeader));

    if (ec) {
      return ec;
    }

    ec = sendAll(message.data(), message.length());

    return ec;
  }

  return returnErrorState();
}

std::error_code TCPConnection::recvExact(void *dataRecv,
                                         const std::size_t sizeData) {
  if (state_ == State::Connected) {
    std::size_t offset = 0;
    auto *byte = static_cast<char *>(dataRecv);

    while (offset < sizeData) {
      const ssize_t result =
          recv(socket_.getFd(), byte + offset, sizeData - offset, 0);

      if (result == -1) {
        int error = errno;

        if (error == EINTR) {
          continue;
        }

        lastError_ = {error, std::generic_category()};
        socket_.reset();
        state_ = State::Failed;

        return lastError_;
      }

      if (result == 0) {
        socket_.reset();
        state_ = State::Closed;

        return std::make_error_code(std::errc::not_connected);
      }

      offset += static_cast<std::size_t>(result);
    }

    return {};
  }

  return returnErrorState();
}

std::error_code TCPConnection::recvMessage(std::vector<char> &buffer) {
  if (state_ == State::Connected) {
    uint32_t messageHeader;

    std::error_code ec = recvExact(&messageHeader, sizeof(messageHeader));

    if (ec) {
      return ec;
    }

    const std::size_t sizeBuffer = ntohl(messageHeader);

    if (sizeBuffer > protocol::MaxMessageSize) {
      lastError_ = std::make_error_code(std::errc::message_size);
      socket_.reset();
      state_ = State::Failed;
      return lastError_;
    }
    buffer.resize(sizeBuffer);

    ec = recvExact(buffer.data(), sizeBuffer);

    if (ec) {
      return ec;
    }

    return {};
  }

  return returnErrorState();
}

std::error_code TCPConnection::returnErrorState() const noexcept {
  if (state_ == State::Failed) {
    return lastError_;
  }

  if (state_ == State::Closed) {
    return std::make_error_code(std::errc::not_connected);
  }

  return {};
}
