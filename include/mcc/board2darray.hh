#include <array>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "mcc/move.hh"
#include "mcc/piece.hh"

namespace mcc {
struct Board2DArray {
  using MoveSet = std::unordered_set<Move, move_hash>;
  std::array<Piece, 64> state;

  Board2DArray(std::string fen);

  void processFEN(std::string fen);

  void setPieceAt(size_t rank, size_t file, Piece piece);
  Piece getPieceAt(size_t rank, size_t file) const;
  Piece getPieceAt(size_t square) const;

  MoveSet generateLegalMoves(const PieceColor& activeColor,
                             const size_t& enPassantSquare,
                             const bool& canEnPassant) const;

  Piece makeMove(const Move& move, const size_t& enPassantSquare);

 private:
  MoveSet generateLegalPawnMoves(const size_t& square,
                                 const size_t& enPassantSquare,
                                 const bool& canEnPassant) const;

  size_t rank(size_t square) const { return square / 8; }
  size_t file(size_t square) const { return square % 8; }

  size_t square(size_t rank, size_t file) const { return 8 * rank + file; }
};
}  // namespace mcc