#pragma once

#include "mcc/common.hh"
#include "mcc/helpers.hh"

#include <array>
#include <bits/utility.h>
#include <cstdint>
#include <type_traits>

namespace mcc {
using lookup_table = std::array<uint64_t, 64>;
using colour_lookup_table = lookup_table[2];

template <Colour colour>
constexpr inline lookup_table pawn_quiet_attack_board_helper = []() {
  lookup_table lut = {};

  constexpr int direction = (colour == Colour::White) ? -1 : 1;

  constexpr int first_row_start = (colour == Colour::White) ? 48 : 8;
  constexpr int first_row_end = (colour == Colour::White) ? 55 : 15;

  const auto first_pawn = 8;
  const auto last_pawn = 55;
  for (int from = first_pawn; from <= last_pawn; ++from) {
    const int to_one_step = from + direction * 8;
    lut[from] |= 1UL << to_one_step;

    if (first_row_start <= from && from <= first_row_end) {
      const int to_two_steps = from + direction * 16;
      lut[from] |= 1UL << to_two_steps;
    }
  }

  return lut;
}();

template <Colour colour>
constexpr inline lookup_table pawn_capture_attack_board_helper = []() {
  lookup_table lut = {};

  constexpr int direction = (colour == Colour::White) ? -1 : 1;

  constexpr int left_row = (colour == Colour::White) ? 0 : 7;
  constexpr int right_row = (colour == Colour::White) ? 7 : 0;

  const auto first_pawn = 8;
  const auto last_pawn = 55;
  for (int from = first_pawn; from <= last_pawn; ++from) {
    // Can only capture to the left if not on a-file
    if ((from % 8 != left_row)) {
      const int to_left = from + direction * 9;
      lut[from] |= 1UL << to_left;
    }

    if ((from % 8 != right_row)) {
      const int to_right = from + direction * 7;
      lut[from] |= 1UL << to_right;
    }
  }

  return lut;
}();

constexpr inline colour_lookup_table pawn_quiet_attack_board = {
    pawn_quiet_attack_board_helper<Colour::White>,
    pawn_quiet_attack_board_helper<Colour::Black>,
};

constexpr inline colour_lookup_table pawn_capture_attack_board = {
    pawn_capture_attack_board_helper<Colour::White>,
    pawn_capture_attack_board_helper<Colour::Black>,
};

constexpr inline lookup_table knight_attack_board = []() {
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

constexpr inline lookup_table king_attack_board = []() {
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
}; // namespace mcc
