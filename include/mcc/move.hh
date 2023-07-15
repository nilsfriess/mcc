#pragma once

#include "mcc/common.hh"
#include "mcc/common/colour.hh"
#include "mcc/common/piece.hh"

#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <type_traits>

namespace mcc {
/*
  Internally, the move is stored in an unsigned 32bit integer, structured as
  follows:

  Upper 16 bits:
  | Reserved (7 bits) | Colour (1 bit) | Piece (6 bits) |

  Lower 16 bits:
  | From (6 bits) | To (6 bits) | Flags (6 bits) |

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

  The Flags are
    - None = 1
    - Capture = 2,
    - PromotionKnight = 4,
    - PromotionBishop = 8,
    - PromotionRook = 16,
    - PromotionQueen = 32
 */

class Move {
  constexpr static uint32_t colour_shift = 24;
  constexpr static uint32_t piece_shift = 18;
  constexpr static uint32_t from_shift = 12;
  constexpr static uint32_t to_shift = 6;

public:
  enum Flags {
    None = 1,
    Capture = 2,
    PromotionKnight = 4,
    PromotionBishop = 8,
    PromotionRook = 16,
    PromotionQueen = 32
  };

  Move(int from, int to, Piece piece, Colour colour, Flags flags = Flags::None)
      : data{(static_cast<uint32_t>(colour) << colour_shift) |
             (static_cast<uint32_t>(piece) << piece_shift) |
             (static_cast<uint32_t>(from) << from_shift) |
             (static_cast<uint32_t>(to) << to_shift) |
             static_cast<uint32_t>(flags)} {
    assert(is_inside_chessboard(from));
    assert(is_inside_chessboard(to));
  }

  uint32_t get_from() const {
    constexpr auto mask = static_cast<uint32_t>(set_bits<0, 1, 2, 3, 4, 5>());
    return (data >> from_shift) & mask;
  }

  uint32_t get_to() const {
    constexpr auto mask = static_cast<uint32_t>(set_bits<0, 1, 2, 3, 4, 5>());
    return (data >> to_shift) & mask;
  }

  Piece get_piece() const {
    constexpr auto mask = set_bits<0, 1, 2, 3, 4, 5>();
    return static_cast<Piece>((data >> piece_shift) & mask);
  }

  Colour get_colour() const {
    constexpr auto mask = set_bits<0>();
    return static_cast<Colour>((data >> colour_shift) & mask);
  }

  uint8_t get_flags() const { return data & set_bits<0, 1, 2, 3, 4, 5>(); }

  bool is_capture() const { return (data & set_bits<1>()); }

  friend std::ostream &operator<<(std::ostream &out, const Move &m) {
    const auto from_algebraic = from_64_to_algebraic(m.get_from());
    const auto to_algebraic = from_64_to_algebraic(m.get_to());

    out << from_algebraic << " -> " << to_algebraic << " (" << m.get_colour()
        << " " << m.get_piece();

    if (m.get_flags() & static_cast<uint8_t>(Flags::Capture))
      out << ", Capture";

    if (m.get_flags() & (Flags::PromotionKnight | Flags::PromotionBishop |
                         Flags::PromotionQueen | Flags::PromotionRook))
      out << ", Promotion";

    out << ")";

    return out;
  }

private:
  uint32_t data;
};

} // namespace mcc
