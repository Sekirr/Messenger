#include <TCPConnection.hpp>
#include <TCPListener.hpp>

int main() {
  // create server
  Socket socket = {AF_INET, SOCK_STREAM, 0};
  TCPListener server(std::move(socket));
  server.createServerAddress(AF_INET, htons(8080), htonl(INADDR_ANY));
  server.createBind();
  server.listenServer();

  Result clientAccept = server.acceptServer();

  if (clientAccept.hasValue()) {
    TCPConnection serverConnection(std::move(clientAccept.getValue()));
    // create client
    Socket client = {AF_INET, SOCK_STREAM, 0};
    TCPConnection clientConnection(std::move(client));

    clientConnection.createClientSockAddr(AF_INET, htons(8080), "127.0.0.1");

    std::error_code ec = clientConnection.connectTo();

    if (!ec) {
      ec = clientConnection.sendMessage("Hello. This is first message");
      std::cout << "yes";
      if (!ec) {
        std::cout << "yes";
        std::vector<char> recvMessageServer;
        ec = serverConnection.recvMessage(recvMessageServer);

        if (!ec) {
          std::cout << "yes";
          const std::string msgSendServer(recvMessageServer.begin(),
                                          recvMessageServer.end());
          ec = serverConnection.sendMessage(msgSendServer);

          if (!ec) {
            std::cout << "yes";
            std::vector<char> recvMessageClient;
            ec = clientConnection.recvMessage(recvMessageClient);
            if (!ec) {
              if (recvMessageClient == recvMessageServer) {
                std::cout << "Connection";
              }
            }
          }
        }
      }
    }
  }
};
