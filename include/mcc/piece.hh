#pragma once

#include "mcc/enums.hh"

namespace mcc {
struct Piece {
  PieceType type = PieceType::None;
  PieceColor color = PieceColor::White;

  Piece() = default;
  Piece(PieceType t_type, PieceColor t_color = PieceColor::White)
      : type(t_type), color(t_color) {}
  Piece(char fen);

  char toFENChar() const;
};
}  // namespace mcc