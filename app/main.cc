#include <iostream>

#include "mcc/mcc.hh"
#include "seasocks/PrintfLogger.h"
#include "seasocks/Server.h"
#include "seasocks/StringUtil.h"
#include "seasocks/WebSocket.h"

using namespace seasocks;

#include "mcc/mcc.hh"

class EchoHandler : public WebSocket::Handler {
 public:
  virtual void onConnect(WebSocket* connection) override {}

  virtual void onData(WebSocket* connection, const char* c_data) override {
    std::string data(c_data);
    std::cout << "Received data: " << data << '\n';
    connection->send("LEGAL,YES");
  }

  virtual void onDisconnect(WebSocket* /*connection*/) override {}
};

int main() {
  /*  auto logger = std::make_shared<PrintfLogger>(Logger::Level::Info);

   Server server(logger);
   auto handler = std::make_shared<EchoHandler>();
   server.addWebSocketHandler("/", handler);
   server.serve("/dev/null", 5050); */

  mcc::MCC engine;
  std::cout << engine.board << "\n";

  const auto& legalMoves = engine.board.generateLegalMoves();
  for (const auto& move : legalMoves) {
    std::cout << "Move: "
              << "[" << move.from.first << ", " << move.from.second << "] -> ["
              << move.to.first << ", " << move.to.second << "]\n";
  }
}
