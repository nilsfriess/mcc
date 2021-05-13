#pragma once

#include "mcc/enums.hh"

namespace mcc {
struct Piece {
  PieceType type = PieceType::None;
  PieceColor color = PieceColor::White;

  Piece() = default;
  Piece(PieceType type, PieceColor color = PieceColor::White)
      : type(type), color(color) {}
  Piece(char fen);

  char toFENChar() const;
};
}  // namespace mcc