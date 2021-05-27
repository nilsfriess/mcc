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
  std::string fen = "";

  Board(const std::string t_fen =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    if (!currentPosition.processFEN(t_fen)) {
      throw std::invalid_argument("Invalid FEN");
    }
    generateFEN();
  }

  Piece getPieceAt(const Coordinate& square) const {
    return currentPosition.getPieceAt(square);
  }

  /* This function tries to make the provided move.
   * Returns {}, if the move is illegal and the move if the move is legal */
  std::optional<Move> makeMove(const Coordinate& from, const Coordinate& to) {
    const auto move = currentPosition.makeMove(from, to);

    if (!move) {  // move is illegal
      return {};
    } else {  // move is legal
      generateFEN();
      return move;
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
    if (currentPosition.activeColor == PieceColor::White)
      fenStream << 'w';
    else
      fenStream << 'b';
    fenStream << ' ';

    // Third field: Castling availability
    if (currentPosition.whiteCanCastleKingSide) fenStream << 'K';
    if (currentPosition.whiteCanCastleQueenSide) fenStream << 'Q';
    if (currentPosition.blackCanCastleKingSide) fenStream << 'k';
    if (currentPosition.blackCanCastleQueenSide) fenStream << 'q';
    if (!currentPosition.whiteCanCastleKingSide &&
        !currentPosition.whiteCanCastleQueenSide &&
        !currentPosition.blackCanCastleKingSide &&
        !currentPosition.blackCanCastleQueenSide)
      fenStream << '-';
    fenStream << ' ';

    // Fourth field: En passant target square
    if (currentPosition.enPassantSquare)
      fenStream << currentPosition.enPassantSquare.value().toAlgebraic();
    else
      fenStream << '-';
    fenStream << ' ';

    // Fifth field: Halfmove clock (not implemented yet)
    fenStream << currentPosition.halfMoves;
    fenStream << ' ';

    // Sixth field: Fullmove number
    fenStream << currentPosition.fullMoves;

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