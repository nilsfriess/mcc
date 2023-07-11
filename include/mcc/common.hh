#pragma once

#include <cstdint>
#include <string>

namespace mcc {

inline std::string from_64_to_algebraic(std::uint8_t field) {
  const auto file = field % 8;
  const auto rank = 8 - field / 8;

  const char file_as_char = 'a' + file;
  return std::string(1, file_as_char) + std::to_string(rank);
}

/* Convert from file and rank to number within 64 field bitboard.
   Rank and file are both 0-indexed. That means:
   - file == 0 corresponds to the a-file, file == 7 is the h-file.
   - rank == 0 is the rank 1, rank == 7 is the rank 8.
 */
inline int from_algebraic_to_64(std::uint8_t file, std::uint8_t rank) {
  return 8 * (7 - rank) + file;
}

inline int from_algebraic_to_64(std::string_view algebraic) {
  const auto file = static_cast<std::size_t>(algebraic.at(0) - 'a');
  const auto rank = static_cast<std::size_t>(algebraic.at(1) - '1');

  return from_algebraic_to_64(file, rank);
}

inline bool is_inside_chessboard(int num) { return (num >= 0) && (num <= 63); }

inline void set_bit(std::uint64_t *val, std::uint8_t position) {
  *val |= (1UL << position);
}

inline void clear_bit(std::uint64_t *val, std::uint8_t position) {
  *val &= ~(1UL << position);
}

inline bool bit_is_set(std::uint64_t val, std::uint8_t position) {
  return val & (1UL << position);
}

template <int... indices> consteval uint64_t set_bits() {
  return (... | (static_cast<uint64_t>(1) << indices));
}

constexpr auto distance = [](auto square1, auto square2) {
  const int file1 = square1 & 7;
  const int rank1 = square1 >> 3;
  const int file2 = square2 & 7;
  const int rank2 = square2 >> 3;

  const auto rankDist = std::abs(rank2 - rank1);
  const auto fileDist = std::abs(file2 - file1);

  return std::max(rankDist, fileDist);
};

} // namespace mcc
