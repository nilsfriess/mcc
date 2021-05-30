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
class Board2DArray {
 public:
  using MoveSet = std::unordered_set<Move, move_hash>;
  using CoordinateSet = std::unordered_set<Coordinate, coord_hash>;

  std::array<Piece, 64> state;
  MoveSet legalMoves = {};
  CoordinateSet attackedSquares = {};

  std::optional<Coordinate> enPassantSquare = {};
  PieceColor activeColor = PieceColor::White;

  int halfMoves = 0;
  int fullMoves = 0;

  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueenSide = true;

  bool processFEN(std::string fen);

  void setPieceAt(const Coordinate& square, Piece piece);
  Piece getPieceAt(const Coordinate& square) const;

  std::optional<Move> makeMove(const Coordinate& from, const Coordinate& to);

 private:
  void generateLegalMoves();
  void generateAttackedSquares();

  void computeLegalMoves(const Coordinate& square, const Piece& piece);
  void computeAttackedSquares(const Coordinate& square, const Piece& piece);

  MoveSet generatePawnMoves(const Coordinate& square, const Piece& piece) const;
  MoveSet generateKnightMoves(const Coordinate& square,
                              const Piece& piece) const;
  MoveSet generateBishopMoves(const Coordinate& square,
                              const Piece& piece) const;
  MoveSet generateRookMoves(const Coordinate& square, const Piece& piece) const;
  MoveSet generateKingMoves(const Coordinate& square, const Piece& piece) const;

  CoordinateSet generatePawnAttackedSquares(const Coordinate& square,
                                            const Piece& piece) const;
  CoordinateSet generateKnightAttackedSquares(const Coordinate& square,
                                              const Piece& piece) const;
  CoordinateSet generateBishopAttackedSquares(const Coordinate& square,
                                              const Piece& piece) const;
  CoordinateSet generateRookAttackedSquares(const Coordinate& square,
                                            const Piece& piece) const;
  CoordinateSet generateKingAttackedSquares(const Coordinate& square,
                                            const Piece& piece) const;

  bool squareBlockedByOwnPieceOrOutsideBoard(const Coordinate& targetSquare,
                                             const Piece& piece) const;
};
}  // namespace mcc