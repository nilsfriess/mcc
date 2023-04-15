#pragma once

#include "mcc/attack_board.hh"
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
     * Knight moves                            *
     *******************************************/
    auto knights = m_board->knights[active_colour];
    while (knights) {
      uint8_t from = __builtin_ctzl(knights);

      const auto knight_attacks = attack_board<Piece::Knight>[from];
      auto to_fields = knight_attacks & ~occupied_by_own;

      while (to_fields) {
        uint8_t to = __builtin_ctzl(to_fields);

        auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        auto flag = static_cast<move::Flags>(is_capture);

        moves.push_back({from, static_cast<uint8_t>(to), Piece::Knight,
                         active_colour, flag});
        to_fields &= ~(1UL << to);
      }

      knights &= ~(1UL << from);
    }

    /*******************************************
     * King moves                              *
     *******************************************/
    auto king = m_board->king[active_colour];
    if (king) {
      uint8_t from = __builtin_ctzl(king);

      const auto king_attacks = attack_board<Piece::King>[from];
      auto to_fields = king_attacks & ~occupied_by_own;

      while (to_fields) {
        uint8_t to = __builtin_ctzl(to_fields);

        auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        auto flag = static_cast<move::Flags>(is_capture);

        moves.push_back(
            {from, static_cast<uint8_t>(to), Piece::King, active_colour, flag});
        to_fields &= ~(1UL << to);
      }
    }

    /*******************************************
     * Bishop moves                            *
     *******************************************/
    auto bishops = m_board->bishops[active_colour];
    while (bishops) {
      uint8_t from = __builtin_ctzl(bishops);

      const auto bishop_attacks = attack_board<Piece::Bishop>[from];

      auto to_fields = bishop_attacks & ~occupied_by_own;

      while (to_fields) {
        uint8_t to = __builtin_ctzl(to_fields);

        auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        auto flag = static_cast<move::Flags>(is_capture);

        moves.push_back({from, static_cast<uint8_t>(to), Piece::Bishop,
                         active_colour, flag});
        to_fields &= ~(1UL << to);
      }
      bishops &= ~(1UL << from);
    }

    /*******************************************
     * Rook moves                              *
     *******************************************/
    auto rooks = m_board->rooks[active_colour];
    while (rooks) {
      uint8_t from = __builtin_ctzl(rooks);

      const auto rook_attacks = attack_board<Piece::Rook>[from];

      auto to_fields = rook_attacks & ~occupied_by_own;

      while (to_fields) {
        uint8_t to = __builtin_ctzl(to_fields);

        auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        auto flag = static_cast<move::Flags>(is_capture);

        moves.push_back(
            {from, static_cast<uint8_t>(to), Piece::Rook, active_colour, flag});
        to_fields &= ~(1UL << to);
      }
      rooks &= ~(1UL << from);
    }

    /*******************************************
     * Queen moves                             *
     *******************************************/
    auto queens = m_board->queen[active_colour];
    while (queens) {
      uint8_t from = __builtin_ctzl(queens);

      const auto queen_attacks = attack_board<Piece::Queen>[from];

      auto to_fields = queen_attacks & ~occupied_by_own;

      while (to_fields) {
        uint8_t to = __builtin_ctzl(to_fields);

        auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        auto flag = static_cast<move::Flags>(is_capture);

        moves.push_back({from, static_cast<uint8_t>(to), Piece::Queen,
                         active_colour, flag});
        to_fields &= ~(1UL << to);
      }
      queens &= ~(1UL << from);
    }

    return moves;
  }

private:
  board *m_board;
};

}; // namespace mcc
