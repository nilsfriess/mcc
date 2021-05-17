#pragma once

#include <clocale>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "mcc/coordinate.hh"
#include "mcc/enums.hh"
#include "mcc/move.hh"
#include "mcc/piece.hh"

namespace mcc {
template <class BoardRep>
struct Board {
  BoardRep currentPosition;
  std::string fen;

  PieceColor activeColor = PieceColor::White;

  std::optional<Coordinate> enPassantSquare = {};

  unsigned int fullMoves = 1;

  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueenSide = true;

  /* Number of halfmoves since last capture or pawn
   * advance; not implemented yet */
  // int halfMoveClock = 0;

  Board(const std::string t_fen =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
      : currentPosition(t_fen) {
    generateFEN();
  }

  Piece getPieceAt(const Coordinate& square) const {
    return currentPosition.getPieceAt(square);
  }

  /* This function tries to make the provided move.
   * Returns false, if the move is illegal and true if the move is legal */
  bool makeMove(const Coordinate& from, const Coordinate& to) {
    const auto piece = currentPosition.makeMove(from, to);

    if (!piece)  // move is illegal
      return false;
    else {  // move is legal
      generateFEN();
      return true;
    }
  }

 private:
  void generateFEN() {
    std::stringstream fenStream;

    // First field: piece placement
    for (size_t rank = 0; rank < 8; ++rank) {
      size_t emptySquareCounter = 0;
      for (size_t file = 0; file < 8; ++file) {
        const auto& currentPiece = getPieceAt({rank, file});
        if (currentPiece.type != PieceType::None) {
          if (emptySquareCounter != 0) {
            // We previously counted empty squares
            fenStream << emptySquareCounter;
            emptySquareCounter = 0;
          }
          fenStream << currentPiece.toFENChar();
        } else {
          emptySquareCounter++;
          if (file == 7) fenStream << emptySquareCounter;
        }
      }
      if (rank != 7) fenStream << "/";
    }
    fenStream << ' ';

    // Second field: Active color
    if (activeColor == PieceColor::White)
      fenStream << 'w';
    else
      fenStream << 'b';
    fenStream << ' ';

    // Third field: Castling availability
    if (whiteCanCastleKingSide) fenStream << 'K';
    if (whiteCanCastleQueenSide) fenStream << 'Q';
    if (blackCanCastleKingSide) fenStream << 'k';
    if (blackCanCastleQueenSide) fenStream << 'q';
    fenStream << ' ';

    // Fourth field: En passant target square
    if (enPassantSquare)
      fenStream << enPassantSquare.value().toAlgebraic();
    else
      fenStream << '-';
    fenStream << ' ';
    // Fifth field: Halfmove clock (not implemented yet)
    fenStream << '0';
    fenStream << ' ';

    // Sixth field: Fullmove number
    fenStream << fullMoves;

    fen = fenStream.str();
  }
};
}  // namespace mcc

template <class BoardRep>
std::ostream& operator<<(std::ostream& os, const mcc::Board<BoardRep>& board) {
  os << "-------------------------------" << '\n';
  for (size_t rank = 0; rank < 8; ++rank) {
    for (size_t file = 0; file < 8; ++file) {
      if (file == 0) os << " ";
      char pieceAsFEN = board.getPieceAt(rank, file).toFENChar();
      os << pieceAsFEN;
      if (file < 7) os << " | ";
    }
    os << '\n';
    os << "-------------------------------" << '\n';
  }

  return os;
}