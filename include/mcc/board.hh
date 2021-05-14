#pragma once

#include <clocale>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "mcc/enums.hh"
#include "mcc/move.hh"
#include "mcc/piece.hh"

namespace mcc {
template <class BoardRep>
struct Board {
  BoardRep currentPosition;
  PieceColor activeColor = PieceColor::White;

  Move::Coordinate enPassantSquare;  // The value of this variable is only valid
                                     // if `canTakeEnPassant == true`

  unsigned int fullMoves = 1;

  bool canTakeEnPassant = false;
  bool whiteCanCastleKingSide = true;
  bool whiteCanCastleQueenSide = true;
  bool blackCanCastleKingSide = true;
  bool blackCanCastleQueenSide = true;

  std::string fen;

  /* Number of halfmoves since last capture or pawn
   * advance; not implemented yet */
  // int halfMoveClock = 0;

  Board(std::string t_fen =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
      : currentPosition(t_fen) {
    generateFEN();
  }

  Piece getPieceAt(size_t rank, size_t file) const {
    return currentPosition.getPieceAt(rank, file);
  }

  BoardRep::MoveSet generateLegalMoves() const {
    const auto squareNumber =
        8 * enPassantSquare.first + enPassantSquare.second;
    return currentPosition.generateLegalMoves(activeColor, squareNumber,
                                              canTakeEnPassant);
  }

  void makeMove(const Move& move) {
    const auto piece = currentPosition.makeMove(
        move, 8 * enPassantSquare.first + enPassantSquare.second);

    // Check if move allows for possible en-passant capture
    const auto rankDistance = std::abs(static_cast<long int>(move.from.first) -
                                       static_cast<long int>(move.to.first));
    if (piece.type == PieceType::Pawn && rankDistance == 2) {
      canTakeEnPassant = true;
      enPassantSquare.second = move.from.second;
      if (activeColor == PieceColor::White)
        enPassantSquare.first = move.from.first - 1;
      else
        enPassantSquare.first = move.from.first + 1;
    } else {
      canTakeEnPassant = false;
    }

    if (activeColor == PieceColor::Black) {
      activeColor = PieceColor::White;
      fullMoves++;
    } else
      activeColor = PieceColor::Black;

    generateFEN();
  }

  std::string coordinateToAlgebraic(size_t rank, size_t file) const {
    return std::string(1, 'a' + rank) + std::to_string(file + 1);
  }

 private:
  void generateFEN() {
    std::stringstream fenStream;

    // First field: piece placement
    for (size_t rank = 0; rank < 8; ++rank) {
      size_t emptySquareCounter = 0;
      for (size_t file = 0; file < 8; ++file) {
        const auto& currentPiece = getPieceAt(rank, file);
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
    if (canTakeEnPassant)
      fenStream << coordinateToAlgebraic(enPassantSquare.first,
                                         enPassantSquare.second);
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