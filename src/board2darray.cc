#include "mcc/board2darray.hh"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace mcc {

bool Board2DArray::processFEN(std::string fen) {
  state = {};
  std::vector<std::string> fenFields;
  std::stringstream ss(fen);
  std::string temp;

  while (ss >> temp) fenFields.push_back(std::move(temp));

  if (fenFields.size() != 6) return false;

  // Process active color field
  if (fenFields[1] == "w")
    activeColor = PieceColor::White;
  else
    activeColor = PieceColor::Black;

  // Process castling rights
  auto cnt = 0;
  const auto& castlingRights = fenFields[2];
  whiteCanCastleKingSide = false;
  whiteCanCastleQueenSide = false;
  blackCanCastleKingSide = false;
  blackCanCastleQueenSide = false;

  if (castlingRights[cnt] == 'K') {
    whiteCanCastleKingSide = true;
    cnt++;
  }
  if (castlingRights[cnt] == 'Q') {
    whiteCanCastleQueenSide = true;
    cnt++;
  }
  if (castlingRights[cnt] == 'k') {
    blackCanCastleKingSide = true;
    cnt++;
  }
  if (castlingRights[cnt] == 'q') {
    blackCanCastleQueenSide = true;
  }

  // Process en passant square
  const auto& enPassantSquareFEN = fenFields[3];
  if (enPassantSquareFEN == "-")
    enPassantSquare = {};
  else
    enPassantSquare = Coordinate(enPassantSquareFEN);

  // Process half moves
  const auto& halfMovesFEN = fenFields[4];
  halfMoves = std::stoi(halfMovesFEN);

  // Process half moves
  const auto& fullMovesFEN = fenFields[5];
  fullMoves = std::stoi(fullMovesFEN);

  std::vector<std::string> fenRanks;
  ss = std::stringstream(fenFields[0]);
  while (std::getline(ss, temp, '/')) fenRanks.push_back(std::move(temp));

  size_t positionInFen =
      0;  // Save position separately, since files can be skipped in FEN
  for (size_t rank = 0; rank < 8; ++rank) {
    const auto& currentRank = fenRanks[rank];
    positionInFen = 0;
    for (size_t file = 0; file < 8; ++file, ++positionInFen) {
      char curr = currentRank[positionInFen];

      if (curr >= '1' && curr <= '8') {
        int squaresToSkip = curr - '0';
        for (size_t i = 0; i < static_cast<size_t>(squaresToSkip);
             ++i, ++file) {
          setPieceAt({rank, file}, Piece());
        }
        --file;  // The last increment is too much
      } else {
        setPieceAt({rank, file}, Piece(curr));
      }
    }
  }

  generateLegalMoves();
  return true;
}

void Board2DArray::setPieceAt(const Coordinate& square, Piece piece) {
  state[square.to64Position()] = piece;
}

Piece Board2DArray::getPieceAt(const Coordinate& square) const {
  return state[square.to64Position()];
}

std::optional<Move> Board2DArray::makeMove(const Coordinate& from,
                                           const Coordinate& to) {
  Move move(from, to);

  if (legalMoves.contains(move)) {
    // Move is legal, carry it out
    const auto& piece = getPieceAt(from);

    // First check if move allows for en passant capture in the next move
    const auto rankDistance = std::abs(static_cast<long int>(from.rank()) -
                                       static_cast<long int>(to.rank()));
    if (piece.type == PieceType::Pawn && rankDistance == 2) {
      if (activeColor == PieceColor::White) {
        enPassantSquare = Coordinate(from.file(), to.below().rank());
      } else {
        enPassantSquare = Coordinate(from.file(), to.above().rank());
      }
    } else {
      enPassantSquare = {};
    }

    if (getPieceAt(to).type != PieceType::None) move.type = MoveType::Capture;

    setPieceAt(from, Piece(PieceType::None));
    setPieceAt(to, piece);

    if (to == enPassantSquare) {
      // Move is an en passant capture
      setPieceAt({from.rank(), to.file()}, Piece(PieceType::None));
      move.type = MoveType::EnPassantCapture;
    }

    if (activeColor == PieceColor::Black) {
      activeColor = PieceColor::White;
    } else
      activeColor = PieceColor::Black;

    generateLegalMoves();  // Update the possible legal moves

    ++fullMoves;
    return move;
  }

  return {};
}

void Board2DArray::generateLegalMoves() {
  legalMoves.clear();
  for (size_t square = 0; square < 64; ++square) {
    const auto& piece = getPieceAt(square);

    if (piece.type == PieceType::None || piece.color != activeColor) continue;

    switch (piece.type) {
      case PieceType::Pawn: {
        const auto pawnMoves = generatePawnMoves(square, piece);
        legalMoves.insert(std::begin(pawnMoves), std::end(pawnMoves));
        break;
      }

      case PieceType::Knight: {
        const auto knightMoves = generateKnightMoves(square, piece);
        legalMoves.insert(std::begin(knightMoves), std::end(knightMoves));
        break;
      }

      case PieceType::Bishop: {
        break;
      }

      case PieceType::Rook: {
        break;
      }

      case PieceType::Queen: {
        break;
      }

      case PieceType::King: {
        break;
      }

      case PieceType::None:
      default:
        break;
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

Board2DArray::MoveSet Board2DArray::generateKnightMoves(
    const Coordinate& square, const Piece& piece) const {
  MoveSet knightMoves;

  // Manually create all 8 possible moves and check if they make sense
  auto possibleSquares = std::vector<Coordinate>{};

  possibleSquares.push_back(square.above().above().left());
  possibleSquares.push_back(square.above().left().left());
  possibleSquares.push_back(square.above().above().right());
  possibleSquares.push_back(square.above().right().right());
  possibleSquares.push_back(square.below().below().left());
  possibleSquares.push_back(square.below().left().left());
  possibleSquares.push_back(square.below().below().right());
  possibleSquares.push_back(square.below().right().right());

  for (const auto& possibleSquare : possibleSquares) {
    if (!possibleSquare.isOutsideOfBoard()) {
      const auto targetPiece = getPieceAt(possibleSquare);
      if (targetPiece.type == PieceType::None ||
          targetPiece.color != piece.color) {
        // Move is semilegal, check if it's a capture
        auto move = Move{square, possibleSquare};
        if (getPieceAt(possibleSquare).type != PieceType::None &&
            getPieceAt(possibleSquare).color != piece.color)
          move.type = MoveType::Capture;

        knightMoves.insert(move);
      }
    }
  }

  return knightMoves;
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
