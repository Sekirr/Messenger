#include "Socket.hpp"

#include <unistd.h>

Socket::Socket(int domain, int type, int protocol) {
  fd_ = socket(domain, type, protocol);
}

Socket::Socket(Socket &&other) noexcept {
  fd_ = other.fd_;
  other.fd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    reset();
    fd_ = other.fd_;
    other.fd_ = -1;
  }

  return *this;
}

Socket::~Socket() { reset(); }

int Socket::getFd() const noexcept { return fd_; }

void Socket::reset() noexcept {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}
