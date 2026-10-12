
#include <TCPConnection.hpp>
#include <TCPListener.hpp>

int main() {
  std::error_code ec = {};

  Socket socket = {AF_INET, SOCK_STREAM, 0};

  TCPListener server(std::move(socket));
  server.createServerAddress(AF_INET, 8080, htonl(INADDR_ANY));

  if (server.createBind()) {
    if (server.listenServer()) {

      while (true) {
        Result clientAccept = server.acceptServer();

        if (clientAccept.hasValue()) {
          TCPConnection serverConnection(std::move(clientAccept.getValue()));

          while (true) {
            std::vector<char> recvMessageServer;
            ec = serverConnection.recvMessage(recvMessageServer);

            if (ec) {
              break;
            }

            const std::string msgSendServer(recvMessageServer.begin(),
                                            recvMessageServer.end());

            std::cout << msgSendServer << std::endl;
            ec = serverConnection.sendMessage(msgSendServer);

            if (ec) {
              break;
            }
          }
        } else {
          break;
        }
      }
    }
  }

  return 0;
};
