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
  move_generator(board &board) : m_board(board) {}

  std::vector<move> generate_pseudo_legal() const {
    const uint64_t occupied_by_opponent = m_board.get_occupied_by_opponent();
    const uint64_t occupied_by_own = m_board.get_occupied_by_own();
    const uint64_t occupied = m_board.get_occupied();

    const auto active_colour = m_board.active_colour;
    const int direction = (active_colour == Colour::White) ? -1 : +1;
    std::vector<move> moves;

    /*******************************************
     * Pawn moves                              *
     *******************************************/
    auto pawns = m_board.pawns[active_colour];
    while (pawns) {
      const uint8_t from = __builtin_ctzl(pawns);

      // Pawn pushes
      const auto pawn_pushes = pawn_quiet_attack_board[active_colour][from];
      auto to_fields = pawn_pushes & ~occupied;

      /* If no piece occupies the square that we can reach in two steps,
         we make an error here. We use here that a pawn can make two steps
         forward if and only if it can make one step forward and the target
         square is not occupied.
       */
      const auto one_step_forward_field = from + direction * 8;
      const auto two_step_forward_field = from + direction * 16;
      const uint64_t one_step_allowed =
          (to_fields & (1UL << one_step_forward_field)) > 0;
      const uint64_t two_steps_allowed =
          one_step_allowed && (to_fields & (1UL << two_step_forward_field));
      to_fields =
          (to_fields &
           ~(1UL << static_cast<unsigned int>(two_step_forward_field))) |
          (two_steps_allowed << two_step_forward_field);

      while (to_fields) {
        const uint8_t to = __builtin_ctzl(to_fields);

        const auto is_promotion = (active_colour == Colour::White)
                                      ? (to <= 7)
                                      : (56 <= to && to <= 63);
        const auto promotion_flag =
            is_promotion ? move::Flags::Promotion : move::Flags::None;

        moves.emplace_back(
            move{from, to, Piece::Pawn, active_colour, promotion_flag});
        to_fields &= ~(1UL << to);
      }

      // Pawn captures
      const auto pawn_captures = pawn_capture_attack_board[active_colour][from];
      to_fields = pawn_captures & occupied_by_opponent;
      while (to_fields) {
        const uint8_t to = __builtin_ctzl(to_fields);

        const auto is_promotion = (active_colour == Colour::White)
                                      ? (to <= 7)
                                      : (56 <= to && to <= 63);
        const auto promotion_flag =
            is_promotion ? move::Flags::Promotion : move::Flags::None;

        moves.emplace_back(move{from, to, Piece::Pawn, active_colour,
                                move::Flags::Capture | promotion_flag});
        to_fields &= ~(1UL << to);
      }

      pawns &= ~(1UL << from);
    }

    /*******************************************
     * Knight moves                            *
     *******************************************/
    auto knights = m_board.knights[active_colour];
    while (knights) {
      const uint8_t from = __builtin_ctzl(knights);

      const auto knight_attacks = knight_attack_board[from];
      auto to_fields = knight_attacks & ~occupied_by_own;

      while (to_fields) {
        const uint8_t to = __builtin_ctzl(to_fields);

        const auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        const auto flag = static_cast<move::Flags>(is_capture);

        moves.emplace_back(move{from, static_cast<uint8_t>(to), Piece::Knight,
                                active_colour, flag});
        to_fields &= ~(1UL << to);
      }

      knights &= ~(1UL << from);
    }

    /*******************************************
     * King moves                              *
     *******************************************/
    const auto king = m_board.king[active_colour];
    if (king) {
      const uint8_t from = __builtin_ctzl(king);

      const auto king_attacks = king_attack_board[from];
      auto to_fields = king_attacks & ~occupied_by_own;

      while (to_fields) {
        const uint8_t to = __builtin_ctzl(to_fields);

        const auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
        const auto flag = static_cast<move::Flags>(is_capture);

        moves.emplace_back(move{from, static_cast<uint8_t>(to), Piece::King,
                                active_colour, flag});
        to_fields &= ~(1UL << to);
      }

      // Generate castling moves
      if (active_colour == Colour::White) {
        if (m_board.white_can_castle_kingside) {
          assert(from == 60);
          // Check if fields between king and rook are empty
          if (!(bit_is_set(occupied, 61) || bit_is_set(occupied, 62))) {
            moves.emplace_back(
                move{from, 62, Piece::King, active_colour, move::Flags::None});
          }
        }
        if (m_board.white_can_castle_queenside) {
          assert(from == 60);
          // Check if fields between king and rook are empty
          if (!(bit_is_set(occupied, 57) || bit_is_set(occupied, 58) ||
                bit_is_set(occupied, 59))) {
            moves.emplace_back(
                move{from, 58, Piece::King, active_colour, move::Flags::None});
          }
        }
      } else {
        if (m_board.black_can_castle_kingside) {
          assert(from == 4);
          // Check if fields between king and rook are empty
          if (!(bit_is_set(occupied, 5) || bit_is_set(occupied, 6))) {
            moves.emplace_back(
                move{from, 6, Piece::King, active_colour, move::Flags::None});
          }
        }
        if (m_board.black_can_castle_queenside) {
          assert(from == 4);
          // Check if fields between king and rook are empty
          if (!(bit_is_set(occupied, 1) || bit_is_set(occupied, 2) ||
                bit_is_set(occupied, 3))) {
            moves.emplace_back(
                move{from, 2, Piece::King, active_colour, move::Flags::None});
          }
        }
      }
    }

    /*******************************************
     * Sliding piece moves                     *
     *******************************************/
    add_sliding_moves(moves, active_colour, occupied_by_own,
                      occupied_by_opponent);

    return moves;
  }

  inline void add_sliding_moves(std::vector<move> &moves, Colour active_colour,
                                uint64_t occupied_by_own,
                                uint64_t occupied_by_opponent) const {
    auto bishops = m_board.bishops[active_colour];
    while (bishops) {
      const uint8_t from = __builtin_ctzl(bishops);
      add_sliding_moves<Piece::Bishop, Direction::NorthEast>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Bishop, Direction::NorthWest>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Bishop, Direction::SouthEast>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Bishop, Direction::SouthWest>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);

      bishops &= ~(1UL << from);
    }

    auto queens = m_board.queen[active_colour];
    while (queens) {
      const uint8_t from = __builtin_ctzl(queens);
      add_sliding_moves<Piece::Queen, Direction::East>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::West>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::South>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::North>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::NorthEast>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::NorthWest>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::SouthEast>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Queen, Direction::SouthWest>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);

      queens &= ~(1UL << from);
    }

    auto rooks = m_board.rooks[active_colour];
    while (rooks) {
      const uint8_t from = __builtin_ctzl(rooks);
      add_sliding_moves<Piece::Rook, Direction::East>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Rook, Direction::West>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Rook, Direction::South>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);
      add_sliding_moves<Piece::Rook, Direction::North>(
          from, moves, active_colour, occupied_by_own, occupied_by_opponent);

      rooks &= ~(1UL << from);
    }
  }

private:
  board &m_board;

  template <Piece piece, int direction>
  inline void add_sliding_moves(uint8_t from, std::vector<move> &moves,
                                Colour active_colour, uint64_t occupied_by_own,
                                uint64_t occupied_by_opponent) const {

    auto attacks = attack_board_sliding<direction>[from];
    auto blocking = attacks & (occupied_by_opponent | occupied_by_own);
    if (blocking) {
      int first_blocking = 0;
      if constexpr (direction < 0)
        first_blocking = 63 - __builtin_clzl(blocking);
      else
        first_blocking = __builtin_ctzl(blocking);
      attacks ^= attack_board_sliding<direction>[static_cast<std::size_t>(
          first_blocking)];
    }

    while (attacks) {
      const uint8_t to = __builtin_ctzl(attacks);

      if ((1UL << to) & occupied_by_own) {
        attacks &= ~(1UL << to);
        continue;
      }

      const auto is_capture = ((occupied_by_opponent & (1UL << to)) > 0);
      const auto flag = static_cast<move::Flags>(is_capture);

      moves.emplace_back(
          move{from, static_cast<uint8_t>(to), piece, active_colour, flag});
      attacks &= ~(1UL << to);
    }
  }
};

}; // namespace mcc
