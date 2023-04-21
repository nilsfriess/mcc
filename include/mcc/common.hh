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

/* Convert from file and rank to number within 64 field bitboard.
   Rank and file are both 0-indexed. That means:
   - file == 0 corresponds to the a-file, file == 7 is the h-file.
   - rank == 0 is the first file, rank == 7 is the last file.
 */
inline int from_algebraic_to_64(uint8_t file, uint8_t rank) {
  return 8 * (7 - rank) + file;
}

inline int from_algebraic_to_64(std::string_view algebraic) {
  const auto file = static_cast<std::size_t>(algebraic.at(0) - 'a');
  const auto rank = static_cast<std::size_t>(algebraic.at(1) - '1');

  return from_algebraic_to_64(file, rank);
}

inline bool is_inside_chessboard(int num) { return (num >= 0) && (num <= 63); }

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
