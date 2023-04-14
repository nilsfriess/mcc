
#include <bitset>
#include <iostream>

#include <mcc/board.hh>
#include <mcc/common.hh>
#include <mcc/move_generator.hh>

int main() {
  mcc::board board;
  mcc::move_generator mg(&board);

  auto moves = mg.generate_pseudo_legal();

  std::cout << "Found " << moves.size() << " moves: \n";
  for (const auto &move : moves) {
    std::cout << move << "\n";
  }
}
