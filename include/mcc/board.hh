#pragma once

#include <string>

#include "mcc/enums.hh"

namespace mcc {
template <class BoardRep>
struct Board {
  BoardRep currentPosition;

  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueemSide = true;

  PieceColor activeColor = PieceColor::White;

  bool canTakeEnPassant = false;
  std::string enPassantSquare;

  /* Number of halfmoves since last capture or pawn
   * advance; not implemented yet */
  // int halfMoveClock = 0;

  unsigned int fullMoves = 1;
};
}  // namespace mcc