#pragma once

#include <ostream>
namespace mcc {

enum Colour { White = 0, Black = 1 };

enum class Piece {
  Pawn = 1,
  Rook = 2,
  Knight = 4,
  Bishop = 8,
  Queen = 16,
  King = 32
};

inline Colour get_other_colour(Colour colour) {
  return static_cast<Colour>(1 - colour);
}

} // namespace mcc

inline std::ostream &operator<<(std::ostream &out, mcc::Colour colour) {
  out << ((colour == mcc::Colour::White) ? "White" : "Black");
  return out;
}

inline std::string from_64_to_algebraic(uint8_t field) {
  const auto file = field % 8;
  const auto rank = 8 - field / 8;

  const char file_as_char = 'a' + file;
  return std::string(1, file_as_char) + std::to_string(rank);
}

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
    }
    return "";
  };

  out << piece_to_string();
  return out;
}
