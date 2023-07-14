#include "mcc/mcc.hh"

#include <cstddef>
#include <iostream>

// inline std::size_t perft(unsigned int depth, mcc::mcc engine) {
//   if (depth == 0)
//     return 1;

//   std::size_t nodes = 0;

//   std::cout << "\n";
//   const auto moves = engine.generate_pseudo_legal();
//   for (const auto move : moves) {
//     auto occ_before = engine.m_board.get_occupied();
//     std::cout << move << "\n";
//     engine.make_move(move);
//     auto occ_after = engine.m_board.get_occupied();
//     std::cout << occ_before - occ_after << "\n";
//     const auto board_before = engine.m_board;
//     nodes += perft(depth - 1, engine);
//     engine.m_board = board_before;
//   }

//   return nodes;
// }

// inline void perft_display(unsigned int depth, std::size_t expected) {
//   mcc::mcc engine;
//   std::cout << "Computed: " << perft(depth, engine)
//             << ", expected: " << expected << "\n";
// }

// int main() {
//   std::vector<std::size_t> expected = {1,      20,      400,      8902,
//                                        197281, 4865609, 119060324};

//   unsigned int max_depth = 2;
//   for (unsigned int depth = 1; depth <= max_depth; ++depth)
//     perft_display(depth, expected.at(depth));
// }

int main() {
  mcc::mcc engine("8/8/1q1q1q2/2RRR3/1qRKRq2/2RRR3/1q1q1q2/8 w - - 0 1");
  std::cout << engine << "\n";

  engine.generate_moves();
}
