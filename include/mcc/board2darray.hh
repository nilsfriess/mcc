#include <array>
#include <string>
#include <utility>
#include <vector>

#include "mcc/move.hh"
#include "mcc/piece.hh"

namespace mcc {
struct Board2DArray {
  std::array<Piece, 64> state;

  Board2DArray(std::string fen);

  void processFEN(std::string fen);

  void setPieceAt(size_t rank, size_t file, Piece piece);
  Piece getPieceAt(size_t rank, size_t file) const;
  Piece getPieceAt(size_t square) const;

  std::vector<Move> generateLegalMoves() const;

  void makeMove(const Move& move);

 private:
  std::vector<Move> generateLegalPawnMoves(size_t square) const;

  size_t rank(size_t square) const { return square / 8; }
  size_t file(size_t square) const { return square % 8; }
};
}  // namespace mcc