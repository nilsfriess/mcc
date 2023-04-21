
#include <bitset>
#include <iostream>

#include <mcc/board.hh>
#include <mcc/common.hh>
#include <mcc/move_generator.hh>

int main(int argc, char *argv[]) {
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  if (argc == 2) {
    fen = argv[1];
  }
  mcc::board board{fen};
  mcc::move_generator mg(&board);

  auto moves = mg.generate_pseudo_legal();

  std::cout << "Found " << moves.size() << " moves: \n";
  for (const auto &move : moves) {
    std::cout << move << "\n";
  }
}
