#include "mcc/board2darray.hh"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace mcc {
Board2DArray::Board2DArray(std::string fen) : state{} { processFEN(fen); }

void Board2DArray::processFEN(std::string fen) {
  state = {};
  std::vector<std::string> fenFields;
  std::stringstream ss(fen);
  std::string temp;

  while (ss >> temp) fenFields.push_back(std::move(temp));

  std::vector<std::string> fenRanks;
  ss = std::stringstream(fenFields[0]);
  while (std::getline(ss, temp, '/')) fenRanks.push_back(std::move(temp));

  size_t positionInFen =
      0;  // Save position separately, since files can be skipped in FEN
  for (size_t rank = 0; rank < 8; ++rank) {
    std::string currentRank = fenRanks[rank];
    positionInFen = 0;
    for (size_t file = 0; file < 8; ++file, ++positionInFen) {
      char curr = currentRank[positionInFen];

      if (curr >= '1' && curr <= '8') {
        int squaresToSkip = curr - '0';
        for (size_t i = 0; i < static_cast<size_t>(squaresToSkip); ++i, ++file)
          setPieceAt(rank, file, Piece());
      } else {
        setPieceAt(rank, file, Piece(curr));
      }
    }
  }
}

void Board2DArray::setPieceAt(size_t rank, size_t file, Piece piece) {
  size_t pos = 8 * rank + file;
  state[pos] = piece;
}

Piece Board2DArray::getPieceAt(size_t rank, size_t file) const {
  return state[8 * rank + file];
}

Piece Board2DArray::getPieceAt(size_t square) const { return state[square]; }

Piece Board2DArray::makeMove(const Move& move, const size_t& enPassantSquare) {
  const auto& piece = getPieceAt(move.from.first, move.from.second);
  setPieceAt(move.from.first, move.from.second, Piece(PieceType::None));
  setPieceAt(move.to.first, move.to.second, piece);

  if (square(move.to.first, move.to.second) == enPassantSquare) {
    // Move is an en passant capture
    setPieceAt(move.from.first, move.to.second, Piece(PieceType::None));
  }

  return piece;
}

Board2DArray::MoveSet Board2DArray::generateLegalMoves(
    const PieceColor& activeColor, const size_t& enPassantSquare,
    const bool& canEnPassant) const {
  MoveSet legalMoves;
  for (size_t square = 0; square < 64; ++square) {
    if (state[square].color != activeColor) continue;

    if (state[square].type == PieceType::Pawn) {
      auto pawnMoves =
          generateLegalPawnMoves(square, enPassantSquare, canEnPassant);
      legalMoves.insert(std::begin(pawnMoves), std::end(pawnMoves));
    }
  }

  return legalMoves;
}

Board2DArray::MoveSet Board2DArray::generateLegalPawnMoves(
    const size_t& square, const size_t& enPassantSquare,
    const bool& canEnPassant) const {
  MoveSet legalMoves;
  auto pawn = getPieceAt(square);
  int direction = 1;
  if (pawn.color == PieceColor::Black) {
    direction = -1;
  }
  const size_t advanceSquare = square - direction * 8;
  const size_t advanceSquareTwo = square - direction * 16;

  if ((pawn.color == PieceColor::Black && rank(square) == 1) ||
      (pawn.color == PieceColor::White && rank(square) == 6)) {
    // pawn is still at initial position

    if (getPieceAt(advanceSquare).type == PieceType::None &&
        getPieceAt(advanceSquareTwo).type == PieceType::None) {
      // Check two square advance moves
      const Move move({rank(square), file(square)},
                      {rank(advanceSquareTwo), file(advanceSquareTwo)});
      legalMoves.insert(move);
    }
  }

  if (getPieceAt(advanceSquare).type == PieceType::None) {
    // Check one square advance moves
    const Move move({rank(square), file(square)},
                    {rank(advanceSquare), file(advanceSquare)});
    legalMoves.insert(move);
  }

  // Capture moves
  const auto captureLeftSquare = advanceSquare - direction;
  const auto captureRightSquare = advanceSquare + direction;

  /* White can't take to the left on file 0,
   * black can't take left on file 7 */
  if ((pawn.color == PieceColor::White && file(square) > 0) ||
      ((pawn.color == PieceColor::Black && file(square) < 7))) {
    // Normal Captures
    if (getPieceAt(captureLeftSquare).type != PieceType::None &&
        getPieceAt(captureLeftSquare).color != pawn.color) {
      const Move move({rank(square), file(square)},
                      {rank(captureLeftSquare), file(captureLeftSquare)});
      legalMoves.insert(move);
    }

    // En passant capture
    if (canEnPassant && enPassantSquare == captureLeftSquare &&
        getPieceAt(enPassantSquare + direction * 8).color != pawn.color) {
      const Move move({rank(square), file(square)},
                      {rank(captureLeftSquare), file(captureLeftSquare)});
      legalMoves.insert(move);
    }
  }
  /* Black can't take to the right on file 0,
   * white can't take right on file 7 */
  if ((pawn.color == PieceColor::Black && file(square) > 0) ||
      ((pawn.color == PieceColor::White && file(square) < 7))) {
    // normal capture
    if (getPieceAt(captureRightSquare).type != PieceType::None &&
        getPieceAt(captureRightSquare).color != pawn.color) {
      const Move move({rank(square), file(square)},
                      {rank(captureRightSquare), file(captureRightSquare)});
      legalMoves.insert(move);
    }

    // En passant captures
    if (canEnPassant && enPassantSquare == captureRightSquare &&
        getPieceAt(enPassantSquare + direction * 8).color != pawn.color) {
      const Move move({rank(square), file(square)},
                      {rank(captureRightSquare), file(captureRightSquare)});
      legalMoves.insert(move);
    }
  }
  return legalMoves;
}

}  // namespace mcc
