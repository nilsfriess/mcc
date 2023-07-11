#pragma once

#include "colour.hh"

#include <ostream>

namespace mcc {
enum class Piece {
  Pawn = 1,
  Rook = 2,
  Knight = 4,
  Bishop = 8,
  Queen = 16,
  King = 32
};

struct ColouredPiece {
  Piece piece;
  Colour colour;
};

inline char piece_to_unicode(ColouredPiece coloured_piece) {
  const auto piece = coloured_piece.piece;
  const auto colour = coloured_piece.colour;
  using enum Colour;;
  switch (piece) {
  case Piece::Pawn:
    return colour == White ? 'P' : 'p';
  case Piece::Bishop:
    return colour == White ? 'B' : 'b';
  case Piece::Knight:
    return colour == White ? 'N' : 'n';
  case Piece::Rook:
    return colour == White ? 'R' : 'r';
  case Piece::Queen:
    return colour == White ? 'Q' : 'q';
  case Piece::King:
    return colour == White ? 'K' : 'k';
  default:
    __builtin_unreachable();
  }
  __builtin_unreachable();
}

} // namespace mcc

inline std::ostream &operator<<(std::ostream &out, mcc::Piece piece) {
  const auto piece_to_string = [piece]() {
    switch (piece) {
    case mcc::Piece::Pawn:
      return "Pawn";
    case mcc::Piece::Rook:
      return "Rook";
    case mcc::Piece::Knight:
      return "Knight";
    case mcc::Piece::Bishop:
      return "Bishop";
    case mcc::Piece::Queen:
      return "Queen";
    case mcc::Piece::King:
      return "King";
    default:
      __builtin_unreachable();
    }
    return "";
  };

  out << piece_to_string();
  return out;
}
