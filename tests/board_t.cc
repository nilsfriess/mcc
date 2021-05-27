#include "catch2/catch.hpp"
#include "mcc/board.hh"
#include "mcc/board2darray.hh"

using namespace mcc;

TEST_CASE("Generated FEN is the same as provided FEN", "[board]") {
  const auto generatedFEN = [](const std::string FEN) {
    Board<Board2DArray> board(FEN);
    return board.fen;
  };

  const auto FENS = {
      "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",
      "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 5 3",
      "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2",
      "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2",
      "2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4",
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
      "10",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
      "10"};

  for (const auto& fen : FENS) REQUIRE(generatedFEN(fen) == fen);
}

TEST_CASE(
    "Moves that would allow for en passant set the en passant square in the "
    "FEN",
    "[board]") {
  const auto FEN =
      "rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 2";
  const Coordinate from{1, 2}, to{3, 2};

  Board<Board2DArray> board(FEN);
  board.makeMove(from, to);

  REQUIRE(board.fen ==
          "rnbqkbnr/pp1p1ppp/8/2pPp3/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 3");
}

TEST_CASE("Constructor throws when passed invalid FEN", "[board") {
  const auto FENs = {"", "rnbqkbnr/pp1p1ppp/8/2pPp3"};

  for (const auto& fen : FENs) REQUIRE_THROWS(Board<Board2DArray>(fen));
}

TEST_CASE(
    "Given the initial position, there are exactly four legal knight moves",
    "[board") {
  Board<Board2DArray> board;

  const auto knight_f3 = Move{{"g1"}, {"f3"}};
  const auto knight_h3 = Move{{"g1"}, {"h3"}};
  const auto knight_c3 = Move{{"b1"}, {"c3"}};
  const auto knight_a3 = Move{{"b1"}, {"a3"}};

  REQUIRE(board.currentPosition.legalMoves.contains(knight_f3));
  REQUIRE(board.currentPosition.legalMoves.contains(knight_h3));
  REQUIRE(board.currentPosition.legalMoves.contains(knight_c3));
  REQUIRE(board.currentPosition.legalMoves.contains(knight_a3));
}

TEST_CASE("Knight on square d5 on empty board has 8 legal moves", "[board]") {
  Board<Board2DArray> board("8/8/8/3N4/8/8/8/8 w - - 0 1");

  REQUIRE(board.currentPosition.legalMoves.size() == 8);
}

TEST_CASE("After 1. e4 there are 5 legal bishop moves", "[board]") {
  Board<Board2DArray> board;
  board.makeMove({"e2"}, {"e4"});
  board.makeMove({"e7"}, {"e5"});

  const auto bishop_moves = {Move{{"f1"}, {"e2"}}, Move{{"f1"}, {"e2"}},
                             Move{{"f1"}, {"e2"}}, Move{{"f1"}, {"e2"}},
                             Move{{"f1"}, {"e2"}}};

  for (const auto& move : bishop_moves) {
    REQUIRE(board.currentPosition.legalMoves.contains(move));
  }
}

TEST_CASE("A single bishop on e5 has 13 legal moves ", "[board]") {
  Board<Board2DArray> board("8/8/8/4B3/8/8/8/8 w - - 0 1");

  REQUIRE(board.currentPosition.legalMoves.size() == 13);
}

TEST_CASE("A single bishop on a1 has 7 legal moves ", "[board]") {
  Board<Board2DArray> board("8/8/8/8/8/8/8/B7 w - - 0 1");

  REQUIRE(board.currentPosition.legalMoves.size() == 7);
}

TEST_CASE("A single ook on d5 has 14 legal moves", "[board]") {
  Board<Board2DArray> board("8/8/8/3R4/8/8/8/8 w - - 0 1");

  REQUIRE(board.currentPosition.legalMoves.size() == 14);
}

TEST_CASE("There are 20 legal moves from the initial position", "[board]") {
  Board<Board2DArray> board;

  REQUIRE(board.currentPosition.legalMoves.size() == 20);
}
