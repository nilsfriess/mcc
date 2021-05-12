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
  virtual void onConnect(WebSocket* /*connection*/) override {}

  virtual void onData(WebSocket* connection, const char* /*data*/) override {
    connection->send("LEGAL,YES");
  }

  virtual void onDisconnect(WebSocket* /*connection*/) override {}
};

int main() {
  auto logger = std::make_shared<PrintfLogger>(Logger::Level::Debug);

  Server server(logger);
  auto handler = std::make_shared<EchoHandler>();
  server.addWebSocketHandler("/", handler);
  server.serve("/dev/null", 5050);

  mcc::MCC engine;
}