#pragma once

#include <cstddef>
#include <utility>

#include "mcc/coordinate.hh"

namespace mcc {
enum class MoveType { Normal, Capture, EnPassantCapture };

struct Move {
  Coordinate from;
  Coordinate to;

  MoveType type = MoveType::Normal;

  Move() = default;
  Move(Coordinate t_from, Coordinate t_to) : from(t_from), to(t_to) {}

  bool operator==(const Move& other) const {
    return (from.rank() == other.from.rank()) &&
           (from.file() == other.from.file()) &&
           (to.rank() == other.to.rank()) && (to.file() == other.to.file());
  }
};

struct move_hash {
  std::size_t operator()(const Move& move) const {
    const auto h1 = std::hash<size_t>{}(move.from.rank());
    const auto h2 = std::hash<size_t>{}(move.from.file());
    const auto h3 = std::hash<size_t>{}(move.to.rank());
    const auto h4 = std::hash<size_t>{}(move.to.file());

    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
  }
};
}  // namespace mcc