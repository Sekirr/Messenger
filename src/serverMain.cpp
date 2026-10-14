
#include <TCPConnection.hpp>
#include <TCPListener.hpp>
#include <thread>
#include <vector>

static void handleClient(TCPConnection clientConnection) {
  std::error_code ec{};
  while (true) {
    std::vector<char> recvMessageServer;
    ec = clientConnection.recvMessage(recvMessageServer);

    if (ec) {
      return;
    }

    const std::string msgSendServer(recvMessageServer.begin(),
                                    recvMessageServer.end());

    std::cout << msgSendServer << std::endl;
    ec = clientConnection.sendMessage(msgSendServer);

    if (ec) {
      return;
    }
  }
}

int main() {
  std::error_code ec = {};

  Socket socket = {AF_INET, SOCK_STREAM, 0};

  TCPListener server(std::move(socket));
  server.createServerAddress(AF_INET, 8080, htonl(INADDR_ANY));

  if (server.createBind()) {
    if (server.listenServer()) {

      std::vector<std::thread> clientThreads;

      while (true) {
        Result clientAccept = server.acceptServer();

        if (clientAccept.hasValue()) {
          TCPConnection clientConnection(std::move(clientAccept.getValue()));
          clientThreads.emplace_back(handleClient, std::move(clientConnection));

        } else {
          for (auto &thread : clientThreads) {
            if (thread.joinable()) {
              thread.join();
            }
          }
          break;
        }
      }
    }
  }

  return 0;
};
