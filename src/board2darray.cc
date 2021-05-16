#include "mcc/board2darray.hh"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace mcc {

Board2DArray::Board2DArray(std::string fen)
    : state{}, legalMoves{}, enPassantSquare{} {
  processFEN(fen);
  generateLegalMoves();
}

void Board2DArray::processFEN(std::string fen) {
  state = {};
  std::vector<std::string> fenFields;
  std::stringstream ss(fen);
  std::string temp;

  while (ss >> temp) fenFields.push_back(std::move(temp));

  if (fenFields[1] == "w")
    activeColor = PieceColor::White;
  else
    activeColor = PieceColor::Black;

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
          setPieceAt({rank, file}, Piece());
      } else {
        setPieceAt({rank, file}, Piece(curr));
      }
    }
  }
}

void Board2DArray::setPieceAt(const Coordinate& square, Piece piece) {
  state[square.to64Position()] = piece;
}

Piece Board2DArray::getPieceAt(const Coordinate& square) const {
  return state[square.to64Position()];
}

std::optional<Piece> Board2DArray::makeMove(const Coordinate& from,
                                            const Coordinate& to) {
  Move move(from, to);
  if (legalMoves.contains(move)) {
    // Move is legal, carry it out
    const auto& piece = getPieceAt(from);
    setPieceAt(from, Piece(PieceType::None));
    setPieceAt(to, piece);

    if (to == enPassantSquare) {
      // Move is an en passant capture
      setPieceAt({from.rank(), to.file()}, Piece(PieceType::None));
    }

    if (activeColor == PieceColor::Black) {
      activeColor = PieceColor::White;
    } else
      activeColor = PieceColor::Black;

    generateLegalMoves();
    return piece;
  }

  return {};
}

void Board2DArray::generateLegalMoves() {
  legalMoves.clear();
  for (size_t square = 0; square < 64; ++square) {
    const auto& piece = getPieceAt(square);

    if (piece.type == PieceType::None || piece.color != activeColor) continue;

    if (piece.type == PieceType::Pawn) {
      const auto pawnMoves = generatePawnMoves(square, piece);
      legalMoves.insert(std::begin(pawnMoves), std::end(pawnMoves));
    }
  }
}

Board2DArray::MoveSet Board2DArray::generatePawnMoves(
    const Coordinate& square, const Piece& piece) const {
  MoveSet legalPawnMoves;

  // Compute possible squares we can move to
  const auto advanceSquareOne =
      (piece.color == PieceColor::White) ? square.above() : square.below();
  const auto advanceSquareTwo = (piece.color == PieceColor::White)
                                    ? advanceSquareOne.above()
                                    : advanceSquareOne.below();

  if ((piece.color == PieceColor::Black && square.rank() == 1) ||
      (piece.color == PieceColor::White && square.rank() == 6)) {
    // pawn is still at initial position

    if (getPieceAt(advanceSquareOne).type == PieceType::None &&
        getPieceAt(advanceSquareTwo).type == PieceType::None) {
      // Check two square advance moves
      const Move move(square, advanceSquareTwo);
      legalPawnMoves.insert(move);
    }
  }

  if (getPieceAt(advanceSquareOne).type == PieceType::None) {
    // Check one square advance moves
    const Move move(square, advanceSquareOne);
    legalPawnMoves.insert(move);
  }

  // Capture moves
  const auto captureLeftSquare = (piece.color == PieceColor::White)
                                     ? advanceSquareOne.left()
                                     : advanceSquareOne.right();
  const auto captureRightSquare = (piece.color == PieceColor::White)
                                      ? advanceSquareOne.right()
                                      : advanceSquareOne.left();

  /* White can't take to the left on file 0,
   * black can't take left on file 7 */
  if ((piece.color == PieceColor::White && square.file() > 0) ||
      ((piece.color == PieceColor::Black && square.file() < 7))) {
    // Normal Captures
    if (getPieceAt(captureLeftSquare).type != PieceType::None &&
        getPieceAt(captureLeftSquare).color != piece.color) {
      const Move move(square, captureLeftSquare);
      legalPawnMoves.insert(move);
    }
  }

  /* Black can't take to the right on file 0,
   * white can't take right on file 7 */
  if ((piece.color == PieceColor::Black && square.file() > 0) ||
      ((piece.color == PieceColor::White && square.file() < 7))) {
    // normal capture
    if (getPieceAt(captureRightSquare).type != PieceType::None &&
        getPieceAt(captureRightSquare).color != piece.color) {
      const Move move(square, captureRightSquare);
      legalPawnMoves.insert(move);
    }
  }

  // // En passant capture
  // if (enPassantSquare) {
  //   if (canEnPassant && enPassantSquare == captureLeftSquare &&
  //       getPieceAt(enPassantSquare + direction * 8).color != pawn.color)
  //       {
  //     const Move move({rank(square), file(square)},
  //                     {rank(captureLeftSquare),
  //                     file(captureLeftSquare)});
  //     legalPawnMoves.insert(move);
  //   }
  // }

  //   // En passant captures
  //   if (canEnPassant && enPassantSquare == captureRightSquare &&
  //       getPieceAt(enPassantSquare + direction * 8).color != pawn.color)
  //       {
  //     const Move move({rank(square), file(square)},
  //                     {rank(captureRightSquare),
  //                     file(captureRightSquare)});
  //     legalPawnMoves.insert(move);
  //   }
  // }

  return legalPawnMoves;
}
}  // namespace mcc

/*// Check if move allows for possible en-passant capture
    const auto rankDistance = std::abs(static_cast<long int>(move.from.rank()) -
                                       static_cast<long int>(move.to.rank()));
    if (piece.type == PieceType::Pawn && rankDistance == 2) {
      canTakeEnPassant = true;
      enPassantSquare.setFile(move.file());
      if (activeColor == PieceColor::White)
        enPassantSquare.setRank(move.from.rank() - 1);
      else
        enPassantSquare.setRank(move.from.rank() + 1);
    } else {
      canTakeEnPassant = false;
    }*/
