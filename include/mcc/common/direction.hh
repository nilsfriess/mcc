#pragma once

#include <array>

namespace mcc {
enum class Direction {
  North = -8,
  NorthEast = -7,
  East = +1,
  SouthEast = +9,
  South = +8,
  SouthWest = +7,
  West = -1,
  NorthWest = -9
};

constexpr std::array<Direction, 8> get_all_directions() {
  using enum Direction;
  return {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
}
} // namespace mcc
