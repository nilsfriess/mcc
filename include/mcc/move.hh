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
};
}  // namespace mcc