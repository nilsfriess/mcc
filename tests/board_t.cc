#include "catch2/catch.hpp"
#include "mcc/board.hh"
#include "mcc/board2darray.hh"

using namespace mcc;

const auto generatedFEN(const std::string FEN) {
  Board<Board2DArray> board(FEN);
  return board.fen;
}

TEST_CASE("Generated FEN is the same as provided FEN", "[board]") {
  const auto FENS = {
      "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",
      "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2",
      "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2",
      "2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4",
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
      "10"};

  for (const auto& fen : FENS) REQUIRE(generatedFEN(fen) == fen);
}
