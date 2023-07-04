#include "mcc/mcc.hh"

#include <cstddef>
#include <iostream>

std::size_t perft(unsigned int depth, const mcc::mcc &engine) {
  if (depth == 0)
    return 1;

  std::size_t nodes = 0;

  const auto moves = engine.generate_pseudo_legal();
  for (const auto move : moves) {
    mcc::mcc engine_copy = engine;
    engine_copy.make_move(move);
    nodes += perft(depth - 1, engine_copy);
  }

  return nodes;
}

void perft_display(unsigned int depth, std::size_t expected) {
  mcc::mcc engine;
  std::cout << "Computed: " << perft(depth, engine)
            << ", expected: " << expected << "\n";
}

int main() {
  std::vector<std::size_t> expected = {1,      20,      400,      8902,
                                       197281, 4865609, 119060324};

  unsigned int max_depth = 6;
  for (unsigned int depth = 1; depth <= max_depth; ++depth)
    perft_display(depth, expected.at(depth));
}
