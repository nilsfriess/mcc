#pragma once

#include "common.hh"
#include "helpers.hh"
#include "move.hh"

#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

namespace mcc {



struct board {
  uint64_t pawns[2] = {0};
  uint64_t rooks[2] = {0};
  uint64_t knights[2] = {0};
  uint64_t bishops[2] = {0};
  uint64_t queen[2] = {0};
  uint64_t king[2] = {0};

  static constexpr int NO_EN_PASSENT = -1;
  int en_passant_square = NO_EN_PASSENT;
  Colour active_colour;

  bool white_can_castle_kingside = false;
  bool white_can_castle_queenside = false;
  bool black_can_castle_kingside = false;
  bool black_can_castle_queenside = false;

  unsigned int half_moves = 0;
  unsigned int full_moves = 0;

  board(std::string fen =
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    load_from_fen(fen);
  }

  bool load_from_fen(std::string fen) {
    std::vector<std::string> fenFields;
    std::stringstream ss{fen};
    std::string temp;

    while (ss >> temp)
      fenFields.push_back(std::move(temp));

    if (fenFields.size() != 6)
      return false;

    // Process active colour field
    if (fenFields[1] == "w")
      active_colour = Colour::White;
    else
      active_colour = Colour::Black;

    // Process castling rights
    std::size_t cnt = 0;
    const auto &castlingRights = fenFields[2];
    white_can_castle_kingside = false;
    white_can_castle_queenside = false;
    black_can_castle_kingside = false;
    black_can_castle_queenside = false;

    if (castlingRights[cnt] == 'K') {
      white_can_castle_kingside = true;
      cnt++;
    }
    if (castlingRights[cnt] == 'Q') {
      white_can_castle_queenside = true;
      cnt++;
    }
    if (castlingRights[cnt] == 'k') {
      black_can_castle_kingside = true;
      cnt++;
    }
    if (castlingRights[cnt] == 'q') {
      black_can_castle_queenside = true;
    }

    // Process en passant square
    const auto en_passant_square_fen = fenFields[3];
    if (en_passant_square_fen == "-")
      en_passant_square = NO_EN_PASSENT;
    else {
      en_passant_square = from_algebraic_to_64(en_passant_square_fen);
      if (!is_inside_chessboard(en_passant_square))
        return false;
    }

    // Process half moves
    const auto &halfMovesFEN = fenFields[4];
    half_moves = static_cast<unsigned int>(std::stoi(halfMovesFEN));

    // Process half moves
    const auto &fullMovesFEN = fenFields[5];
    full_moves = static_cast<unsigned int>(std::stoi(fullMovesFEN));

    // Process pieces
    std::vector<std::string> fenRanks;
    ss = std::stringstream(fenFields[0]);
    while (std::getline(ss, temp, '/'))
      fenRanks.push_back(std::move(temp));

    // Save position separately, since files can be skipped in FEN
    size_t position_in_fen = 0;
    for (size_t rank = 0; rank < 8; ++rank) {
      const auto current_rank = fenRanks[rank];
      position_in_fen = 0;
      for (size_t file = 0; file < 8; ++file, ++position_in_fen) {
        char curr = current_rank[position_in_fen];
        if (curr >= '1' && curr <= '8') {
          int squaresToSkip = curr - '0';
          file += static_cast<std::size_t>(squaresToSkip - 1);
        } else {
          // Set piece at current file and rank. Our rank is zero indexed, but
          // first rank in fen is rank 8
          if (!set_piece_at(file, 7 - rank, curr))
            return false;
        }
      }
    }

    return true;
  }

  // bool make_move_from_algebraic(std::string_view move) {
  //   auto from = from_algebraic_to_64(move.substr(0, 2));
  //   auto to = from_algebraic_to_64(move.substr(2, 2));

  //   uint64_t *board = nullptr;
  //   if (bit_is_set(pawns[active_colour], from))
  //     board = &(pawns[active_colour]);
  //   if (bit_is_set(knights[active_colour], from))
  //     board = &(knights[active_colour]);
  //   if (bit_is_set(bishops[active_colour], from))
  //     board = &(bishops[active_colour]);
  //   if (bit_is_set(rooks[active_colour], from))
  //     board = &(rooks[active_colour]);
  //   if (bit_is_set(queen[active_colour], from))
  //     board = &(queen[active_colour]);
  //   if (bit_is_set(king[active_colour], from))
  //     board = &(king[active_colour]);

  //   clear_bit(*board, from);
  //   set_bit(*board, to);

  //   if (bit_is_set(get_occupied_by_opponent(), to)) {
  //     // Move iss a capture, remove piece of opponent
  //     auto opponent_colour = get_other_colour(active_colour);
  //     uint64_t *opponent_board = nullptr;
  //     if (bit_is_set(pawns[opponent_colour], to))
  //       opponent_board = &(pawns[opponent_colour]);
  //     if (bit_is_set(knights[opponent_colour], to))
  //       opponent_board = &(knights[opponent_colour]);
  //     if (bit_is_set(bishops[opponent_colour], to))
  //       opponent_board = &(bishops[opponent_colour]);
  //     if (bit_is_set(rooks[opponent_colour], to))
  //       opponent_board = &(rooks[opponent_colour]);
  //     if (bit_is_set(queen[opponent_colour], to))
  //       opponent_board = &(queen[opponent_colour]);
  //     if (bit_is_set(king[opponent_colour], to))
  //       opponent_board = &(king[opponent_colour]);

  //     clear_bit(*opponent_board, to);
  //   }

  //   active_colour = get_other_colour(active_colour);

  //   return true;
  // }

  // Make the given move. Does not check if the move is legal or even
  // pseudolegal.
  void make_move(move m) {
    auto piece = m.get_piece();
    auto colour = m.get_colour();

    assert(colour == active_colour);

    uint64_t *move_board = nullptr;
    switch (piece) {
    case Piece::Pawn:
      move_board = &(pawns[colour]);
      break;
    case Piece::Rook:
      move_board = &(rooks[colour]);

      if (m.get_colour() == Colour::White) {
        const bool is_kingside_rook = m.get_from() == 63;
        const bool is_queenside_rook = m.get_from() == 56;

        if (is_kingside_rook)
          white_can_castle_kingside = false;
        if (is_queenside_rook)
          white_can_castle_queenside = false;
      } else {
        const bool is_kingside_rook = m.get_from() == 7;
        const bool is_queenside_rook = m.get_from() == 0;

        if (is_kingside_rook)
          black_can_castle_kingside = false;
        if (is_queenside_rook)
          black_can_castle_queenside = false;
      }

      break;
    case Piece::Queen:
      move_board = &(queen[colour]);
      break;
    case Piece::Bishop:
      move_board = &(bishops[colour]);
      break;
    case Piece::Knight:
      move_board = &(knights[colour]);
      break;
    case Piece::King:
      move_board = &(king[colour]);
      if (active_colour == White) {
        white_can_castle_kingside = false;
        white_can_castle_queenside = false;
      } else {
        black_can_castle_kingside = false;
        black_can_castle_queenside = false;
      }
      break;
    default:
      __builtin_unreachable();
    };

    assert(move_board);

    set_bit(move_board, m.get_to());
    clear_bit(move_board, m.get_from());

    if (m.is_capture()) {
      auto other_colour = get_other_colour(colour);

      if (bit_is_set(pawns[other_colour], m.get_to()))
        clear_bit(&(pawns[other_colour]), m.get_to());

      if (bit_is_set(knights[other_colour], m.get_to()))
        clear_bit(&(knights[other_colour]), m.get_to());

      if (bit_is_set(bishops[other_colour], m.get_to()))
        clear_bit(&(bishops[other_colour]), m.get_to());

      if (bit_is_set(rooks[other_colour], m.get_to()))
        clear_bit(&(rooks[other_colour]), m.get_to());

      if (bit_is_set(queen[other_colour], m.get_to()))
        clear_bit(&(queen[other_colour]), m.get_to());
    }

    // Check if move is castling
    if (m.get_piece() == Piece::King) {
      // Check white castling
      if ((colour == Colour::White) && (m.get_from() == 60)) {
        if (m.get_to() == 62) {
          // Kingside castling
          assert(bit_is_set(rooks[colour], 63));
          clear_bit(&(rooks[colour]), 63);
          set_bit(&(rooks[colour]), 61);
        } else if (m.get_to() == 58) {
          // Queenside castling
          assert(bit_is_set(rooks[colour], 56));
          clear_bit(&(rooks[colour]), 56);
          set_bit(&(rooks[colour]), 59);
        }
      } else if((colour == Colour::White) && (m.get_from() == 4)) {
        if (m.get_to() == 6) {
          // Kingside castling
          assert(bit_is_set(rooks[colour], 7));
          clear_bit(&(rooks[colour]), 7);
          set_bit(&(rooks[colour]), 5);
        } else if (m.get_to() == 2) {
          // Queenside castling
          assert(bit_is_set(rooks[colour], 0));
          clear_bit(&(rooks[colour]), 0);
          set_bit(&(rooks[colour]), 3);
        }

      }
    }

    active_colour = get_other_colour(colour);
  }

  uint64_t get_occupied(Colour colour) const {
    return pawns[colour] | rooks[colour] | knights[colour] | bishops[colour] |
           queen[colour] | king[colour];
  }

  uint64_t get_occupied_by_opponent() const {
    return get_occupied(get_other_colour(active_colour));
  }

  uint64_t get_occupied_by_own() const { return get_occupied(active_colour); }

  uint64_t get_occupied() const {
    return get_occupied_by_own() | get_occupied_by_opponent();
  }

  bool is_field_occupied(uint64_t pieces, size_t field) const {
    return pieces & (static_cast<uint64_t>(1) << field);
  }

  

  uint64_t *get_bitboard(Piece piece, Colour colour) {
    uint64_t *bitboard;
    switch (piece) {
    case Piece::Pawn:
      bitboard = &(pawns[colour]);
      break;

    case Piece::Rook:
      bitboard = &(rooks[colour]);
      break;

    case Piece::Knight:
      bitboard = &(knights[colour]);
      break;

    case Piece::Bishop:
      bitboard = &(bishops[colour]);
      break;

    case Piece::Queen:
      bitboard = &(queen[colour]);
      break;

    case Piece::King:
      bitboard = &(king[colour]);
      break;

    default:
      __builtin_unreachable();
    }

    return bitboard;
  }
};

}; // namespace mcc
