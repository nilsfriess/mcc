#include <array>

#include "mcc/piece.hh"

namespace mcc {
struct Board2DArray {
  std::array<Piece, 64> state;
};
}  // namespace mcc