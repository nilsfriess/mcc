#pragma once

#include "common.hh"
#include "helpers.hh"
#include "move.hh"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>

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

  static constexpr int no_en_passant = -1;
  int en_passant_square = no_en_passant;
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
      en_passant_square = no_en_passant;
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

    // Process piecesstd::vector<std::string> fenRanks;
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

  bool make_move(std::string_view move) {
    auto from = from_algebraic_to_64(move.substr(0, 2));
    auto to = from_algebraic_to_64(move.substr(2, 2));

    uint64_t *board = nullptr;
    if (bit_is_set(pawns[active_colour], from))
      board = &(pawns[active_colour]);
    if (bit_is_set(knights[active_colour], from))
      board = &(knights[active_colour]);
    if (bit_is_set(bishops[active_colour], from))
      board = &(bishops[active_colour]);
    if (bit_is_set(rooks[active_colour], from))
      board = &(rooks[active_colour]);
    if (bit_is_set(queen[active_colour], from))
      board = &(queen[active_colour]);
    if (bit_is_set(king[active_colour], from))
      board = &(king[active_colour]);

    clear_bit(*board, from);
    set_bit(*board, to);

    if (bit_is_set(get_occupied_by_opponent(), to)) {
      // Move iss a capture, remove piece of opponent
      auto opponent_colour = get_other_colour(active_colour);
      uint64_t *opponent_board = nullptr;
      if (bit_is_set(pawns[opponent_colour], to))
        opponent_board = &(pawns[opponent_colour]);
      if (bit_is_set(knights[opponent_colour], to))
        opponent_board = &(knights[opponent_colour]);
      if (bit_is_set(bishops[opponent_colour], to))
        opponent_board = &(bishops[opponent_colour]);
      if (bit_is_set(rooks[opponent_colour], to))
        opponent_board = &(rooks[opponent_colour]);
      if (bit_is_set(queen[opponent_colour], to))
        opponent_board = &(queen[opponent_colour]);
      if (bit_is_set(king[opponent_colour], to))
        opponent_board = &(king[opponent_colour]);

      clear_bit(*opponent_board, to);
    }

    active_colour = get_other_colour(active_colour);

    return true;
  }

  bool make_move(move m) {
    auto piece = m.get_piece();
    auto colour = m.get_colour();

    uint64_t *board = nullptr;
    switch (piece) {
    case Piece::Pawn:
      board = &(pawns[colour]);
      break;
    case Piece::Rook:
      board = &(rooks[colour]);
      break;
    case Piece::Queen:
      board = &(queen[colour]);
      break;
    case Piece::Bishop:
      board = &(bishops[colour]);
      break;
    case Piece::Knight:
      board = &(knights[colour]);
      break;
    case Piece::King:
      board = &(king[colour]);
      break;
    };

    set_bit(*board, m.get_to());
    clear_bit(*board, m.get_from());

    if (m.is_capture()) {
      auto other_colour = get_other_colour(active_colour);

      if (bit_is_set(pawns[other_colour], m.get_to())) {
        clear_bit(pawns[other_colour], m.get_to());
        return true;
      }

      if (bit_is_set(knights[other_colour], m.get_to())) {
        clear_bit(knights[other_colour], m.get_to());
        return true;
      }

      if (bit_is_set(bishops[other_colour], m.get_to())) {
        clear_bit(bishops[other_colour], m.get_to());
        return true;
      }

      if (bit_is_set(rooks[other_colour], m.get_to())) {
        clear_bit(rooks[other_colour], m.get_to());
        return true;
      }

      if (bit_is_set(queen[other_colour], m.get_to())) {
        clear_bit(queen[other_colour], m.get_to());
        return true;
      }
    }

    // TODO: We are currently not checking if this is a valid move
    return true;
  }

  uint64_t get_occupied(Colour colour) const {
    return pawns[colour] | rooks[colour] | knights[colour] | bishops[colour] |
           queen[colour] | king[colour];
  }

  uint64_t get_occupied_by_opponent() const {
    return get_occupied(get_other_colour(active_colour));
  }

  uint64_t get_occupied_by_own() const { return get_occupied(active_colour); }

  inline bool is_field_occupied(uint64_t pieces, size_t field) const {
    return pieces & (static_cast<uint64_t>(1) << field);
  }

  bool set_piece_at(size_t file, size_t rank, char piece) {
    auto field = from_algebraic_to_64(file, rank);
    if (!is_inside_chessboard(field))
      return false;

    switch (piece) {
    case 'p':
      pawns[Colour::Black] |= (1UL << field);
      break;

    case 'P':
      pawns[Colour::White] |= (1UL << field);
      break;

    case 'r':
      rooks[Colour::Black] |= (1UL << field);
      break;

    case 'R':
      rooks[Colour::White] |= (1UL << field);
      break;

    case 'n':
      knights[Colour::Black] |= (1UL << field);
      break;

    case 'N':
      knights[Colour::White] |= (1UL << field);
      break;

    case 'b':
      bishops[Colour::Black] |= (1UL << field);
      break;

    case 'B':
      bishops[Colour::White] |= (1UL << field);
      break;

    case 'q':
      queen[Colour::Black] |= (1UL << field);
      break;

    case 'Q':
      queen[Colour::White] |= (1UL << field);
      break;

    case 'k':
      king[Colour::Black] |= (1UL << field);
      break;

    case 'K':
      king[Colour::White] |= (1UL << field);
      break;

    default:
      return false;
    }

    return true;
  }

  inline uint64_t get_bitboard(Piece piece, Colour colour) const {
    uint64_t bitboard = 0;
    switch (piece) {
    case Piece::Pawn:
      bitboard = pawns[colour];
      break;

    case Piece::Rook:
      bitboard = rooks[colour];
      break;

    case Piece::Knight:
      bitboard = knights[colour];
      break;

    case Piece::Bishop:
      bitboard = bishops[colour];
      break;

    case Piece::Queen:
      bitboard = queen[colour];
      break;

    case Piece::King:
      bitboard = king[colour];
      break;

    default:
      bitboard = 0;
    }

    return bitboard;
  }
};

}; // namespace mcc
