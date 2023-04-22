#pragma once

#include "mcc/board.hh"
#include "mcc/common.hh"
#include "mcc/move.hh"
#include "mcc/move_generator.hh"

#include <filesystem>
#include <fstream>
#include <thread>

namespace mcc {

/* The actual engine class. */
class mcc {
  board m_board;
  move_generator m_generator;

public:
  mcc() : m_board{}, m_generator{&m_board} {}

  std::vector<move> generate_pseudo_legal() const {
    return m_generator.generate_pseudo_legal();
  }

  bool make_move(move move) {
    m_board.make_move(move);
    m_board.active_colour = get_other_colour(m_board.active_colour);

    return true;
  }

  bool start_uci(std::string log_file = "log.txt") {
    namespace fs = std::filesystem;
    using namespace std::literals;

    auto log_file_path = fs::path("/home/nils/log/mcc") / log_file;
    logger = std::ofstream{log_file_path};
    log("\n\nStarting engine in UCI mode. Waiting for `uci` command...");

    std::string input;
    std::cin >> input;

    if (input == "uci") {
      log_command(input);
      send_to_gui("id mcc");
      send_to_gui("uciok");
    }

    input = "";
    while (input != "quit") {
      std::getline(std::cin, input);

      log_command(input);

      if (input == "isready")
        send_to_gui("readyok");

      if (input.starts_with("position")) {
        // Extract moves
        input = input.substr(9); // remove `position` from received command
        if (input.starts_with("startpos")) {
          m_board = board{};

          input = input.substr(9); // remove `startpos` from received command
        } else { // If we do not receive `startpos`, then we are given a FEN
          // Everything up to the word `moves` is part of the FEN
          auto moves_begin = input.find("moves");
          auto fen = input.substr(0, moves_begin - 1);

          input = input.substr(moves_begin);

          m_board = board{fen};
        }

        if (!input.starts_with("moves")) {
          log("Received something that does not makes sense. Stopping.");
          return false;
        } else {
          input = input.substr(6); // remove `moves` from received command
          log("Now trying to perform moves: " + input);

          std::stringstream ss{input};
          std::vector<std::string> moves;
          std::string tmp;
          while (std::getline(ss, tmp, ' '))
            moves.push_back(std::move(tmp));

          for (const auto &move : moves)
            m_board.make_move(move);
        }
      }

      if (input.starts_with("go")) {
        auto moves = m_generator.generate_pseudo_legal();

        int rand_index = rand() % moves.size();
        auto rand_move = moves[rand_index];

        auto move_algebraic = from_64_to_algebraic(rand_move.get_from()) +
                              from_64_to_algebraic(rand_move.get_to());

        send_to_gui("bestmove " + move_algebraic);
      }
    }

    log("Stopping engine.");
    logger.close();

    return true;
  }

private:
  void send_to_gui(std::string_view command) {
    std::cout << command << "\n";
    log("Sending command to engine: " + std::string(command));
  }

  void log(std::string_view message) { logger << message << std::endl; }
  void log_command(std::string_view command) {
    logger << "Received command: " << command << std::endl;
  }

  std::ofstream logger;
};

}; // namespace mcc
