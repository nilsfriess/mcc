#include <array>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "mcc/coordinate.hh"
#include "mcc/move.hh"
#include "mcc/piece.hh"

namespace mcc {
struct Board2DArray {
  using MoveSet = std::unordered_set<Move, move_hash>;

  std::array<Piece, 64> state;
  MoveSet legalMoves;

  std::optional<Coordinate> enPassantSquare;
  PieceColor activeColor = PieceColor::White;

  int halfMoves = 0;
  int fullMoves = 0;

  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueenSide = true;

  Board2DArray(std::string fen);

  void processFEN(std::string fen);

  void setPieceAt(const Coordinate& square, Piece piece);
  Piece getPieceAt(const Coordinate& square) const;

  std::optional<Piece> makeMove(const Coordinate& from, const Coordinate& to);

 private:
  void generateLegalMoves();

  MoveSet generatePawnMoves(const Coordinate& square, const Piece& piece) const;

  // if this holds a value, an en passant move to the
  // given square if possible
};
}  // namespace mcc