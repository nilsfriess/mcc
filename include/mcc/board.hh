#pragma once

#include "mcc/common.hh"
#include "mcc/coordinates.hh"
#include "mcc/helpers.hh"

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

    // Process active color field
    if (fenFields[1] == "w")
      active_colour = Colour::White;
    else
      active_colour = Colour::Black;

    // Process castling rights
    auto cnt = 0;
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
    half_moves = std::stoi(halfMovesFEN);

    // Process half moves
    const auto &fullMovesFEN = fenFields[5];
    full_moves = std::stoi(fullMovesFEN);

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
          file += squaresToSkip;
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
