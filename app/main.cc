#include <algorithm>
#include <iostream>
#include <sstream>

#include "mcc/mcc.hh"
#include "mcc/move.hh"
#include "seasocks/PrintfLogger.h"
#include "seasocks/Server.h"
#include "seasocks/StringUtil.h"
#include "seasocks/WebSocket.h"

using namespace seasocks;

#include "mcc/mcc.hh"

struct MoveHandler : public WebSocket::Handler {
  const mcc::MCC* engine;

  MoveHandler(const mcc::MCC* engine) : engine(engine) {}

  virtual void onConnect(WebSocket* connection) override {}

  virtual void onData(WebSocket* connection, const char* c_data) override {
    const std::string data(c_data);
    std::cout << "Received data: " << data << '\n';

    std::vector<std::string> parameters;
    std::stringstream param_stream(data);

    std::string tmp;
    while (std::getline(param_stream, tmp, ',')) parameters.push_back(tmp);

    if (parameters[0] == "MOVE") {
      std::cout << "Checking if move is legal... ";

      const mcc::Move move({stoi(parameters[1]), stoi(parameters[2])},
                           {stoi(parameters[3]), stoi(parameters[4])});

      std::cout << "\n" << engine->board << "\n";

      const auto& legalMoves = engine->board.generateLegalMoves();
      for (const auto& move : legalMoves) {
        std::cout << "Move: "
                  << "[" << move.from.first << ", " << move.from.second
                  << "] -> [" << move.to.first << ", " << move.to.second
                  << "]\n";
      }

      if (std::find(legalMoves.begin(), legalMoves.end(), move) !=
          legalMoves.end()) {
        std::cout << "Move was legal!\n";
        connection->send("LEGAL,YES");
      } else {
        std::cout << "Move was illegal!\n";
        connection->send("LEGAL,NO");
      }
    }
  }

  virtual void onDisconnect(WebSocket* /*connection*/) override {}
};

int main() {
  mcc::MCC engine;
  std::cout << engine.board;

  auto handler = std::make_shared<MoveHandler>(&engine);
  auto logger = std::make_shared<PrintfLogger>(Logger::Level::Info);
  Server server(logger);

  server.addWebSocketHandler("/", handler);
  server.serve("/dev/null", 5050);
}
