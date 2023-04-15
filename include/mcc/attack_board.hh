#pragma once

#include "mcc/common.hh"
#include "mcc/helpers.hh"

#include <array>
#include <bits/utility.h>
#include <cstdint>
#include <type_traits>

namespace mcc {
using lookup_table = std::array<uint64_t, 64>;

template <Piece piece> static constexpr lookup_table attack_board = {};

template <>
constexpr inline lookup_table attack_board<Piece::Knight> = []() {
  lookup_table lut = {};

  constexpr std::array<int, 8> to_diffs = {-15, 15, -17, 17, -10, 10, -6, 6};
  for (int from = 0; from < 64; ++from) {
    for (const auto &to_diff : to_diffs) {
      const int to = from + to_diff;
      if ((to < 0) or (to > 63))
        continue;
      if (distance(from, to) > 2)
        continue;
      lut[from] |= 1UL << to;
    }
  }
  return lut;
}();

template <>
constexpr inline lookup_table attack_board<Piece::King> = []() {
  lookup_table lut = {};

  constexpr std::array<int, 8> to_diffs = {-9, -8, -7, -1, 1, 7, 8, 9};
  for (int from = 0; from < 64; ++from) {
    for (const auto &to_diff : to_diffs) {
      const int to = from + to_diff;
      if ((to < 0) or (to > 63))
        continue;
      if (distance(from, to) > 1)
        continue;
      lut[from] |= 1UL << to;
    }
  }
  return lut;
}();

namespace Direction {
static constexpr int North = -8;
static constexpr int East = 1;
static constexpr int West = -1;
static constexpr int South = 8;
static constexpr int NorthWest = -9;
static constexpr int NorthEast = -7;
static constexpr int SouthWest = 7;
static constexpr int SouthEast = 9;
} // namespace Direction

template <int... directions>
constexpr auto attack_board_sliding = []() {
  lookup_table lut = {};

  for (int from = 0; from < 64; ++from) {
    (
        [&] {
          int multiplier = 1;
          int to = from + multiplier * directions;
          int to_before = from;

          while (to >= 0 and to <= 63) {
            if (distance(to, to_before) > 1)
              break;

            lut[from] |= 1UL << to;
            ++multiplier;
            to_before = to;
            to = from + multiplier * directions;
          }
        }(),
        ...);
  }
  return lut;
}();

template <>
constexpr inline lookup_table attack_board<Piece::Bishop> =
    attack_board_sliding<-7, 7, -9, 9>;

template <>
constexpr inline lookup_table attack_board<Piece::Rook> =
    attack_board_sliding<1, -1, 8, -8>;

template <>
constexpr inline lookup_table attack_board<Piece::Queen> =
    attack_board_sliding<1, -1, 8, -8, -7, 7, -9, 9>;

}; // namespace mcc
