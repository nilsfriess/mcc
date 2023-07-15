#pragma once

#include "mcc/common.hh"
#include "mcc/common/colour.hh"
#include "mcc/common/direction.hh"
#include "mcc/common/piece.hh"
#include "mcc/move.hh"

#include <bit>
#include <bitset>
#include <exception>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/*
  Board is represented by six arrays uint64_t piece[2], where
  piece[mcc::Colour::White] represents the white pieces and
  piece[mcc::Colour::Black] represents the black pieces.

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

namespace mcc {
constexpr int NO_EN_PASSANT = -1;

class mcc {
  // Piece bitboards
  // piece[Colour::White] -> White pieces
  // piece[Colour::Black] -> Black pieces
  uint64_t pawns[2] = {0};
  uint64_t rooks[2] = {0};
  uint64_t knights[2] = {0};
  uint64_t bishops[2] = {0};
  uint64_t queens[2] = {0};
  uint64_t king[2] = {0};

  int en_passant_square = NO_EN_PASSANT;
  Colour active_colour = Colour::White;

  bool white_can_castle_kingside = false;
  bool white_can_castle_queenside = false;
  bool black_can_castle_kingside = false;
  bool black_can_castle_queenside = false;

  unsigned int half_moves = 0;
  unsigned int full_moves = 0;

public:
  mcc(const std::string &fen =
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    load_from_fen(fen);
  }

  std::optional<ColouredPiece> get_piece_at(std::size_t file,
                                            std::size_t rank) const {
    using enum Colour;
    using enum Piece;

    const auto position = from_algebraic_to_64(file, rank);
    if (not is_inside_chessboard(position))
      return {};

    if (bit_is_set(pawns[White], position))
      return ColouredPiece{Pawn, White};
    if (bit_is_set(pawns[Black], position))
      return ColouredPiece{Pawn, Black};

    if (bit_is_set(knights[White], position))
      return ColouredPiece{Knight, White};
    if (bit_is_set(knights[Black], position))
      return ColouredPiece{Knight, Black};

    if (bit_is_set(bishops[White], position))
      return ColouredPiece{Bishop, White};
    if (bit_is_set(bishops[Black], position))
      return ColouredPiece{Bishop, Black};

    if (bit_is_set(rooks[White], position))
      return ColouredPiece{Rook, White};
    if (bit_is_set(rooks[Black], position))
      return ColouredPiece{Rook, Black};

    if (bit_is_set(king[White], position))
      return ColouredPiece{King, White};
    if (bit_is_set(king[Black], position))
      return ColouredPiece{King, Black};

    if (bit_is_set(queens[White], position))
      return ColouredPiece{Queen, White};
    if (bit_is_set(queens[Black], position))
      return ColouredPiece{Queen, Black};

    return {};
  }

  /* Performs the given move form position `from` to position `to`.
     Throws an exception if either of the positions (or both) are outside the
     board or if there is no piece at the position `from`. Otherwise does not
     check for legality of the move.
   */
  void make_move(std::uint8_t from, std::uint8_t to) {
    if (not(is_inside_chessboard(from) && is_inside_chessboard(to)))
      throw std::invalid_argument(
          "[mcc::make_move] At least one of the given positions is invalid.");

    Colour colours[] = {Colour::White, Colour::Black};
    uint64_t *bitboards[] = {pawns, knights, bishops, rooks, queens, king};

    for (auto colour : colours) {
      for (auto &bitboard : bitboards) {
        auto &current_bitboard = bitboard[colour];

        if (bit_is_set(current_bitboard, from)) {
          clear_bit(&current_bitboard, from);
          set_bit(&current_bitboard, to);

          // Check if move is capture
          for (auto &other_bitboards : bitboards) {
            auto &other_bitboard = bitboard[get_other_colour(colour)];

            if (bit_is_set(other_bitboard, to))
              clear_bit(&other_bitboard, to);
          }

          return;
        }
      }
    }

    // If we reach this, the there was no piece at the position `from`
    throw std::invalid_argument("[mcc::make_move] No piece at given positon.");
  }

  // Generates all legal moves
  std::vector<Move> generate_moves() const {
    std::vector<Move> moves;

    ////////////////////////////////////
    // Step 1: Identify pinned pieces //
    ////////////////////////////////////

    /* This map holds the position of pinned pieces as the keys and the
     * direction along which it is pinned (ie. along which it is allowed to
     * move) as the value. */
    std::unordered_map<unsigned int, Direction> pinned_positions;
    for (auto direction : get_all_directions()) {
      if (auto position = get_pinned_in_dir(direction))
        pinned_positions.insert({position.value(), direction});
    }

    generate_pawn_moves(moves, pinned_positions);

    return moves;
  }

private:
  uint64_t occupied_by(Colour colour) const {
    return pawns[colour] | knights[colour] | bishops[colour] | queens[colour] |
           rooks[colour] | king[colour];
  }

  bool load_from_fen(const std::string &fen) {
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
      en_passant_square = NO_EN_PASSANT;
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
      queens[Colour::Black] |= (1UL << field);
      break;

    case 'Q':
      queens[Colour::White] |= (1UL << field);
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

  std::optional<unsigned int> get_pinned_in_dir(Direction dir) const {
    using enum Direction;

    const auto occ_by_own = pawns[active_colour] | rooks[active_colour] |
                            knights[active_colour] | bishops[active_colour] |
                            queens[active_colour];
    const auto king_pos = std::countr_zero(king[active_colour]);

    const int ray_dir = static_cast<int>(dir);

    // Compute ray starting from king position
    uint64_t ray = 0;
    {
      int curr = king_pos + ray_dir;
      while (true) {
        set_bit(&ray, curr);
        int next_curr = curr + ray_dir;

        if (not is_inside_chessboard(curr) or distance(curr, next_curr) > 1)
          break;
        curr = next_curr;
      }
    }

    if (auto pieces_on_ray = ray & occ_by_own) {
      // There is at least one of our pieces on the ray, check if it is
      // actually pinned
      const auto other_colour = get_other_colour(active_colour);

      uint64_t attackers = queens[other_colour];
      if (dir == North || dir == South || dir == East || dir == West)
        attackers |= rooks[other_colour];
      else if (dir == NorthEast || dir == NorthWest || dir == SouthEast ||
               dir == SouthWest)
        attackers |= bishops[other_colour];

      if (attackers & ray) {
        // Get position of the first piece on the ray.
        const auto piece_on_ray = 63 - std::countl_zero(pieces_on_ray);

        // We already know that there is an attacker on the ray.
        // We still have to check if one of our own pieces or a non-attacking
        // piece blocks the pin.
        bool blocked = false;

        const auto occ_by_other = occupied_by(other_colour);
        const auto non_attackers = occ_by_other & ~attackers;
        const auto blockers = non_attackers | occ_by_own;

        {
          int curr = piece_on_ray + ray_dir;
          while (not bit_is_set(attackers, curr)) { // follow ray until attacker
            if (bit_is_set(blockers, curr))
              blocked = true;
            curr += ray_dir;
          }
        }

        /* Since `piece_on_ray` is not necessarily the piece closest to the
           king, we also have to follow the ray in the other direction and check
           for blockers. */
        {
          int curr = piece_on_ray - ray_dir;
          // follow ray until we reach king
          while (not bit_is_set(king[active_colour], curr)) {
            if (bit_is_set(blockers, curr))
              blocked = true;
            curr -= ray_dir;
          }
        }

        if (not blocked) {
          // We found a pinned piece, add it to the map
          return piece_on_ray;
        }
      }
    }
    return {};
  }

  void generate_pawn_moves(
      std::vector<Move> &moves,
      [[maybe_unused]] const std::unordered_map<unsigned int, Direction>
          &pinned_positions) const {
    const auto occ_by_own = occupied_by(active_colour);
    const auto occ_by_other = occupied_by(get_other_colour(active_colour));
    const auto occ = occ_by_own | occ_by_other;

    const int direction = active_colour == Colour::White ? -1 : 1;

    auto rem_pawns = this->pawns[active_colour];
    
    while (rem_pawns) {
      const auto from = std::countr_zero(rem_pawns);

      int to = from + direction * 8;
      if (not bit_is_set(occ, to)) {
        /* Check if moving to last rank (-> promotion). We check this by moving
         * another step forward and see if we land outside the board. */
        if (not is_inside_chessboard(to + direction * 8)) {
          moves.push_back(Move{from, to, Piece::Pawn, active_colour,
                               Move::Flags::PromotionBishop});
          moves.push_back(Move{from, to, Piece::Pawn, active_colour,
                               Move::Flags::PromotionKnight});
          moves.push_back(Move{from, to, Piece::Pawn, active_colour,
                               Move::Flags::PromotionRook});
          moves.push_back(Move{from, to, Piece::Pawn, active_colour,
                               Move::Flags::PromotionQueen});
        } else {
          moves.push_back(Move{from, to, Piece::Pawn, active_colour});
        }
      }

      // We are done considering the current pawn, delete from temp bitboard
      rem_pawns &= ~(1UL << static_cast<uint8_t>(from));
    }
  }
};

}; // namespace mcc

inline std::ostream &operator<<(std::ostream &out, const mcc::mcc &board) {
  out << "    ---------------------------------\n";
  for (std::size_t i = 0; i < 8; ++i) {
    auto rank = 7 - i;
    out << (rank + 1) << " ";
    out << " "
        << " | ";
    for (std::size_t file = 0; file < 8; ++file) {
      if (auto coloured_piece = board.get_piece_at(file, rank)) {
        out << mcc::piece_to_unicode(coloured_piece.value());
        out << " | ";
      } else {
        out << " "
            << " | ";
      }
    }
    out << "\n    ---------------------------------";
    out << "\n";
  }
  out << "      a   b   c   d   e   f   g   h\n";

  return out;
}
