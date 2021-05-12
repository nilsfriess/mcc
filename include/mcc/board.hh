#pragma once

#include <clocale>
#include <iostream>
#include <string>

#include "mcc/enums.hh"
#include "mcc/piece.hh"

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

  Board(std::string fen =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
      : currentPosition(fen) {}

  Piece getPieceAt(size_t rank, size_t file) const {
    return currentPosition.getPieceAt(rank, file);
  }
};
}  // namespace mcc

template <class BoardRep>
std::ostream& operator<<(std::ostream& os, const mcc::Board<BoardRep>& board) {
  setlocale(LC_ALL, "");
  os << "-------------------------------" << '\n';
  for (size_t rank = 0; rank < 8; ++rank) {
    for (size_t file = 0; file < 8; ++file) {
      if (file == 0) os << " ";
      char pieceAsFEN = board.getPieceAt(rank, file).toFENChar();
      os << pieceAsFEN;
      if (file < 7) os << " | ";
    }
    std::cout << '\n';
    os << "-------------------------------" << '\n';
  }

  return os;
}