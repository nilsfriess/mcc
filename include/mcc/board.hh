#pragma once

#include "mcc/common.hh"
#include "mcc/coordinates.hh"
#include "mcc/helpers.hh"

#include <cstdint>
#include <iostream>
#include <string_view>

namespace mcc {

/*
  Class that represents the board with its pieces.
  Stores 6 arrays uint64_t piece[2], where piece[mcc::Colour::White]
  represents the white pieces and piece[mcc::Colour::Black] represents
  the black pieces.

  The indexing works as follows
                   BLACK
    -----------------------------------------
 8  | 0  | 1  | 2  |  3 | 4  | 5  | 6  | 7  |
    -----------------------------------------
 7  | 8  |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
    -----------------------------------------
 6  | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
    -----------------------------------------
 5  | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
    -----------------------------------------
 4  | 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
    -----------------------------------------
 3  | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
    -----------------------------------------
 2  | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 |
    -----------------------------------------
 1  | 56 | 57 | 58 | 59 | 60 | 61 | 62 | 63 |
    -----------------------------------------
.      a    b    c    d    e    f    g    h
                   WHITE

 */

struct board {
  uint64_t pawns[2] = {0};
  uint64_t rooks[2] = {0};
  uint64_t knights[2] = {0};
  uint64_t bishops[2] = {0};
  uint64_t queen[2] = {0};
  uint64_t king[2] = {0};

  int en_passant_square = -1;
  Colour active_colour;

  board() { load_initial(); };

  void load_initial() {
    pawns[Colour::White] = set_bits<48, 49, 50, 51, 52, 53, 54, 55>();
    pawns[Colour::Black] = set_bits<8, 9, 10, 11, 12, 13, 14, 15>();

    rooks[Colour::White] = set_bits<56, 63>();
    rooks[Colour::Black] = set_bits<0, 7>();

    knights[Colour::White] = set_bits<27>();
    knights[Colour::Black] = set_bits<1, 6>();

    bishops[Colour::White] = set_bits<58, 61>();
    bishops[Colour::Black] = set_bits<2, 5>();

    queen[Colour::White] = set_bits<59>();
    queen[Colour::Black] = set_bits<3>();

    king[Colour::White] = set_bits<60>();
    king[Colour::Black] = set_bits<4>();

    active_colour = Colour::White;
  }

  uint64_t get_occupied(Colour color) const {
    return pawns[color] | rooks[color] | knights[color] | bishops[color] |
           queen[color] | king[color];
  }

  uint64_t get_occupied_by_opponent() const {
    return get_occupied(get_other_colour(active_colour));
  }

  uint64_t get_occupied_by_own() const { return get_occupied(active_colour); }

  inline bool is_field_occupied(uint64_t pieces, size_t field) const {
    return pieces & (static_cast<uint64_t>(1) << field);
  }

  inline uint64_t get_bitboard(Piece piece, Colour color) const {
    uint64_t bitboard = 0;
    switch (piece) {
    case Piece::Pawn:
      bitboard = pawns[color];
      break;

    case Piece::Rook:
      bitboard = rooks[color];
      break;

    case Piece::Knight:
      bitboard = knights[color];
      break;

    case Piece::Bishop:
      bitboard = bishops[color];
      break;

    case Piece::Queen:
      bitboard = queen[color];
      break;

    case Piece::King:
      bitboard = king[color];
      break;

    default:
      bitboard = 0;
    }

    return bitboard;
  }
};

}; // namespace mcc
