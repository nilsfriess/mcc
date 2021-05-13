#pragma once

#include <cstddef>
#include <utility>

namespace mcc {
struct Move {
  using Coordinate = std::pair<size_t, size_t>;

  Coordinate from;
  Coordinate to;

  Move() = default;
  Move(Coordinate from, Coordinate to) : from(from), to(to) {}

  bool operator==(const Move& other) const {
    return (from.first == other.from.first) &&
           (from.second == other.from.second) && (to.first == other.to.first) &&
           (to.second == other.to.second);
  }
};
}  // namespace mcc