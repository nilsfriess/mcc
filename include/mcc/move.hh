#pragma once

#include "mcc/common.hh"
#include "mcc/common/colour.hh"
#include "mcc/common/piece.hh"

#include <bitset>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <type_traits>

namespace mcc {
/*
  Internally, the move is stored in an unsigned 32bit integer, structured as
  follows:

  Upper 16 bits:
  | Reserved (11 bits) | Colour (1 bit) | Piece (6 bits) |

  Lower 16 bits:
  | From (6 bits) | To (6 bits) | Flags (4 bits) |

  The pieces are encoded as
    - Pawn   = 1
    - Rook   = 2
    - Knight = 4
    - Bishop = 8
    - Queen  = 16
    - King   = 32

  The Colours are encoded as
    - White = 0
    - Black = 1

  Thus, the move 1. e4 is encoded as (without the reserved bits)
  0     1     52     36    0
  0 00001 110100 100100 0000
 */

class Move {
  constexpr static uint32_t colour_shift = 22;
  constexpr static uint32_t piece_shift = 16;
  constexpr static uint32_t from_shift = 10;
  constexpr static uint32_t to_shift = 4;

public:
  enum class Flags : uint8_t { None = 0, Capture = 2, Promotion = 4 };

  Move(uint8_t from, uint8_t to, Piece piece, Colour colour,
       Flags flags = Flags::None)
      : data{(static_cast<uint32_t>(colour) << colour_shift) |
             (static_cast<uint32_t>(piece) << piece_shift) |
             (static_cast<uint32_t>(from) << from_shift) |
             (static_cast<uint32_t>(to) << to_shift) |
             static_cast<uint32_t>(flags)} {}

  uint32_t get_from() const {
    constexpr auto mask = static_cast<uint32_t>(set_bits<0, 1, 2, 3, 4, 5>());
    return (data >> from_shift) & mask;
  }

  uint32_t get_to() const {
    constexpr auto mask = static_cast<uint32_t>(set_bits<0, 1, 2, 3, 4, 5>());
    return (data >> to_shift) & mask;
  }

  Piece get_piece() const {
    constexpr auto piece_mask = set_bits<16, 17, 18, 19, 20, 21>();
    return static_cast<Piece>((data & piece_mask) >> piece_shift);
  }

  Colour get_colour() const {
    constexpr auto colour_mask = set_bits<22>();
    return static_cast<Colour>((data & colour_mask) >> colour_shift);
  }

  uint8_t get_flags() const {
    return static_cast<uint8_t>(data & set_bits<0, 1, 2, 3>());
  }

  bool is_capture() const { return (data & set_bits<1>()); }

  friend std::ostream &operator<<(std::ostream &out, const Move &m) {
    const auto from_algebraic = from_64_to_algebraic(m.get_from());
    const auto to_algebraic = from_64_to_algebraic(m.get_to());

    out << from_algebraic << " -> " << to_algebraic << " (" << m.get_colour()
        << " " << m.get_piece();

    if (m.get_flags() & static_cast<uint8_t>(Flags::Capture))
      out << ", Capture";

    if (m.get_flags() & static_cast<uint8_t>(Flags::Promotion))
      out << ", Promotion";

    out << ")";

    return out;
  }

private:
  uint32_t data;
};

inline Move::Flags operator|(Move::Flags lhs, Move::Flags rhs) {
  using T = std::underlying_type_t<Move::Flags>;
  return static_cast<Move::Flags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline Move::Flags &operator|=(Move::Flags &lhs, Move::Flags rhs) {
  lhs = lhs | rhs;
  return lhs;
}

} // namespace mcc
