#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

template <int... indices> consteval uint64_t set_bits() {
  return (... | (static_cast<uint64_t>(1) << indices));
}

constexpr auto distance = [](int square1, int square2) {
  int file1 = square1 & 7;
  int rank1 = square1 >> 3;
  int file2 = square2 & 7;
  int rank2 = square2 >> 3;

  auto rankDist = std::abs(rank2 - rank1);
  auto fileDist = std::abs(file2 - file1);

  return std::max(rankDist, fileDist);
};
