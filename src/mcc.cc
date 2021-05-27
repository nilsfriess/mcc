#include "mcc/mcc.hh"

#include "mcc/move.hh"

namespace mcc {
std::optional<Move> MCC::makeMove(Coordinate from, Coordinate to) {
  return board.makeMove(from, to);
}

}  // namespace mcc