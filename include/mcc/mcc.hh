#pragma once

#include <optional>

#include "mcc/board.hh"
#include "mcc/board2darray.hh"
#include "mcc/move.hh"

namespace mcc {
class MCC {
  Board<Board2DArray> board;

 public:
  std::optional<Move> makeMove(Coordinate from, Coordinate to);
  std::string fen() const { return board.fen; }

  void reset();
};
}  // namespace mcc