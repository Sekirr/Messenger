
#include <TCPConnection.hpp>
#include <TCPListener.hpp>

int main() {

  // create client
  Socket client = {AF_INET, SOCK_STREAM, 0};
  TCPConnection clientConnection(std::move(client));

  std::error_code ec =
      clientConnection.createClientSockAddr(AF_INET, 8080, "127.0.0.1");

  if (!ec) {
    ec = clientConnection.connectTo();

    if (!ec) {
      ec = clientConnection.sendMessage("Hello. This is first message");

      if (!ec) {
        std::vector<char> recvMessageClient;
        ec = clientConnection.recvMessage(recvMessageClient);
        std::cout.write(recvMessageClient.data(), recvMessageClient.size());
        std::cout << std::endl;
      }
    }
  }

  return 0;
};
