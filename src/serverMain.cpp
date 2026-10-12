
#include <TCPConnection.hpp>
#include <TCPListener.hpp>

int main() {
  Socket socket = {AF_INET, SOCK_STREAM, 0};
  TCPListener server(std::move(socket));
  server.createServerAddress(AF_INET, 8080, htonl(INADDR_ANY));
  if (server.createBind()) {
    if (server.listenServer()) {
      while (true) {
        Result clientAccept = server.acceptServer();

        std::cout << clientAccept.hasValue() << std::endl;

        if (clientAccept.hasValue()) {
          TCPConnection serverConnection(std::move(clientAccept.getValue()));

          std::vector<char> recvMessageServer;
          std::error_code ec = serverConnection.recvMessage(recvMessageServer);

          if (!ec) {
            const std::string msgSendServer(recvMessageServer.begin(),
                                            recvMessageServer.end());
            ec = serverConnection.sendMessage(msgSendServer);
          } else {
            break;
          }
          std::cout.write(recvMessageServer.data(), recvMessageServer.size());
          std::cout << std::endl;
        } else {
          break;
        }
      }
    }
  }

  return 0;
};
