#include <array>
#include <string>
#include <vector>

#include "mcc/piece.hh"

namespace mcc {
struct Board2DArray {
  std::array<Piece, 64> state;

  Board2DArray(std::string fen);

  void processFEN(std::string fen);

  void setPieceAt(size_t rank, size_t file, Piece piece);
  Piece getPieceAt(size_t rank, size_t file) const;
};
}  // namespace mcc