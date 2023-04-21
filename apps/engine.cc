
#include <bitset>
#include <iostream>

#include <mcc/mcc.hh>

int main(int argc, char *argv[]) {
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  if (argc == 2) {
    fen = argv[1];
  }

  mcc::mcc engine{fen};
  auto moves = engine.generate_pseudo_legal();

  std::cout << "Found " << moves.size() << " moves: \n";
  for (const auto &move : moves) {
    std::cout << move << "\n";
  }
}
