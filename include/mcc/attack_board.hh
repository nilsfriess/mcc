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
constexpr lookup_table attack_board<Piece::Knight> = []() {
  lookup_table lut = {};

  constexpr std::array<int, 8> to_diffs = {-15, 15, -17, 17, -10, 10, -6, 6};
  for (int from = 0; from < 64; ++from) {
    for (const auto &to_diff : to_diffs) {
      int to = from + to_diff;
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
constexpr lookup_table attack_board<Piece::King> = []() {
  lookup_table lut = {};

  constexpr std::array<int, 8> to_diffs = {-9, -8, -7, -1, 1, 7, 8, 9};
  for (int from = 0; from < 64; ++from) {
    for (const auto &to_diff : to_diffs) {
      int to = from + to_diff;
      if ((to < 0) or (to > 63))
        continue;
      if (distance(from, to) > 1)
        continue;
      lut[from] |= 1UL << to;
    }
  }
  return lut;
}();

template <int... directions>
constexpr auto attack_board_sliding = []() {
  lookup_table lut = {};

  for (int from = 0; from < 64; ++from) {
    (
        [&] {
          int multiplier = 1;
          int to = from + multiplier * directions;

          while (distance(from, to) == multiplier) {
            if ((to < 0) or (to > 63))
              break;

            lut[from] |= 1UL << to;
            ++multiplier;
            auto to_before = to;
            to = from + multiplier * directions;
            if (distance(to, to_before) > 1)
              break;
          }
        }(),
        ...);
  }
  return lut;
};

template <>
constexpr lookup_table attack_board<Piece::Bishop> =
    attack_board_sliding<-7, 7, -9, 9>();

template <>
constexpr lookup_table attack_board<Piece::Rook> =
    attack_board_sliding<1, -1, 8, -8>();

template <>
constexpr lookup_table attack_board<Piece::Queen> =
    attack_board_sliding<1, -1, 8, -8, -7, 7, -9, 9>();

}; // namespace mcc
