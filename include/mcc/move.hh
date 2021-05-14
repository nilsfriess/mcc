#pragma once

#include <cstddef>
#include <utility>

namespace mcc {
struct Move {
  using Coordinate = std::pair<size_t, size_t>;

  Coordinate from;
  Coordinate to;

  Move() = default;
  Move(Coordinate t_from, Coordinate t_to) : from(t_from), to(t_to) {}

  bool operator==(const Move& other) const {
    return (from.first == other.from.first) &&
           (from.second == other.from.second) && (to.first == other.to.first) &&
           (to.second == other.to.second);
  }
};

struct move_hash {
  std::size_t operator()(const Move& move) const {
    const auto h1 = std::hash<size_t>{}(move.from.first);
    const auto h2 = std::hash<size_t>{}(move.from.second);
    const auto h3 = std::hash<size_t>{}(move.to.first);
    const auto h4 = std::hash<size_t>{}(move.to.second);

    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
  }
};
}  // namespace mcc