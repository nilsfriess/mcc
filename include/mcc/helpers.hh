#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <type_traits>

template <int... indices> consteval uint64_t set_bits() {
  return (... | (static_cast<uint64_t>(1) << indices));
}

// std::abs isn't constexpr, we provide our own
namespace mcc::detail {
template <class T, std::enable_if_t<std::is_arithmetic_v<T>>...>
constexpr auto abs(T const &x) noexcept {
  return x < 0 ? -x : x;
}
} // namespace mcc::detail

constexpr auto distance = [](int square1, int square2) {
  const int file1 = square1 & 7;
  const int rank1 = square1 >> 3;
  const int file2 = square2 & 7;
  const int rank2 = square2 >> 3;

  const auto rankDist = mcc::detail::abs(rank2 - rank1);
  const auto fileDist = mcc::detail::abs(file2 - file1);

  return std::max(rankDist, fileDist);
};
