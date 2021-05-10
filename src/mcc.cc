#include "mcc/mcc.hh"

#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include "seasocks/PrintfLogger.h"
#include "seasocks/Server.h"
#include "seasocks/StringUtil.h"
#include "seasocks/WebSocket.h"

using namespace seasocks;

class EchoHandler : public WebSocket::Handler {
 public:
  virtual void onConnect(WebSocket* /*connection*/) override {}

  virtual void onData(WebSocket* connection, const uint8_t* data,
                      size_t length) override {
    connection->send(data, length);  // binary
  }

  virtual void onData(WebSocket* connection, const char* data) override {
    connection->send(data);  // text
  }

  virtual void onDisconnect(WebSocket* /*connection*/) override {}
};

void mcc::start() {
  auto logger = std::make_shared<PrintfLogger>(Logger::Level::Debug);

  Server server(logger);
  auto handler = std::make_shared<EchoHandler>();
  server.addWebSocketHandler("/", handler);
  server.serve("/dev/null", 8000);
}