#pragma once

#include <array>

namespace mcc {
enum class Direction {
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West,
  NorthWest
};

inline int direction_to_ray_dir(Direction dir) {
  int ray_dir = 0;

  using enum Direction;
  switch (dir) {
  case North:
    ray_dir = -8;
    break;
  case NorthEast:
    ray_dir = -7;
    break;
  case East:
    ray_dir = +1;
    break;
  case SouthEast:
    ray_dir = +9;
    break;
  case South:
    ray_dir = +8;
    break;
  case SouthWest:
    ray_dir = +7;
    break;
  case West:
    ray_dir = -1;
    break;
  case NorthWest:
    ray_dir = -9;
    break;
  default:
    __builtin_unreachable();
  }
  return ray_dir;
}

constexpr std::array<Direction, 8> get_all_directions() {
  using enum Direction;
  return {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
}
} // namespace mcc
