#pragma once

#include "mcc/board.hh"
#include "mcc/common.hh"
#include "mcc/helpers.hh"
#include "mcc/move.hh"

#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

namespace mcc {

class move_generator {
public:
  move_generator(board *board) : m_board(board) {}

  std::vector<move> generate_pseudo_legal() const {
    uint64_t occupied_by_opponent = m_board->get_occupied_by_opponent();
    uint64_t occupied_by_own = m_board->get_occupied_by_own();
    uint64_t occupied = occupied_by_own | occupied_by_opponent;

    auto active_colour = m_board->active_colour;
    uint8_t direction = (active_colour == Colour::White) ? -1 : +1;
    std::vector<move> moves;

    /*******************************************
     * Pawn moves                              *
     *******************************************/
    auto pawns = m_board->pawns[active_colour];
    while (pawns) {
      uint8_t from = __builtin_ctzl(pawns);

      // Non-promotion moves
      if (((active_colour == Colour::White) && (from > 15)) ||
          (active_colour == Colour::Black) && (from < 48)) {

        // 1 step pawn moves
        uint8_t to = from + direction * 8;
        if (!(occupied & (1UL << to))) {
          // Field `to` is empty, add move to possible moves
          moves.push_back(move{from, to, Piece::Pawn, active_colour});
        }

        // 2 step pawn moves
        // Check if pawn is still on initial field
        if ((active_colour == Colour::White) && (48 <= from && from <= 55) ||
            (active_colour == Colour::Black) && (8 <= from && from <= 15)) {
          uint8_t to = from + direction * 16;
          uint8_t between = from + direction * 8;

          if (!(occupied & (1UL << to)) && !(occupied & (1UL << between))) {
            moves.push_back(move{from, to, Piece::Pawn, active_colour});
          }
        }

      } else {
        // TODO: Promotions
      }

      // Pawn captures
      if (active_colour == Colour::White) {
        uint8_t to = from - 9;
        if (occupied_by_opponent & (1UL << to)) {
          // Field `to` is occupied by opponent, we can capture
          moves.push_back(
              move{from, to, Piece::Pawn, active_colour, move::Flags::Capture});
        }
        to = from - 7;
        if (occupied_by_opponent & (1UL << to)) {
          // Field `to` is occupied by opponent, we can capture
          moves.push_back(
              move{from, to, Piece::Pawn, active_colour, move::Flags::Capture});
        }
      } else { // active_colour == Colour::Black
        uint8_t to = from + 9;
        if (occupied_by_opponent & (1UL << to)) {
          // Field `to` is occupied by opponent, we can capture
          moves.push_back(
              move{from, to, Piece::Pawn, active_colour, move::Flags::Capture});
        }
        to = from + 7;
        if (occupied_by_opponent & (1UL << to)) {
          // Field `to` is occupied by opponent, we can capture
          moves.push_back(
              move{from, to, Piece::Pawn, active_colour, move::Flags::Capture});
        }
      }

      pawns &= ~(1UL << from);
    }

    /*******************************************
     * Knight moves                              *
     *******************************************/
    auto knights = m_board->knights[active_colour];
    while (knights) {
      uint8_t from = __builtin_ctzl(knights);

      std::array<int, 8> to_diffs = {-15, 15, -17, 17, -10, 10, -6, 6};

      for (const auto &to_diff : to_diffs) {
        int to = static_cast<int>(from) + to_diff;
        if ((to < 0) or (to > 63))
          continue;

        if (distance(from, to) > 2)
          continue;

        if (occupied_by_own & (1UL << static_cast<uint8_t>(to)))
          continue;

        if (occupied_by_opponent & (1UL << static_cast<uint8_t>(to)))
          moves.push_back(move{from, static_cast<uint8_t>(to), Piece::Knight,
                               active_colour, move::Flags::Capture});
        else
          moves.push_back(move{from, static_cast<uint8_t>(to), Piece::Knight,
                               active_colour});
      }

      knights &= ~(1UL << from);
    }

    return moves;
  }

private:
  board *m_board;
};

}; // namespace mcc
