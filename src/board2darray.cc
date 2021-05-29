#include "mcc/board2darray.hh"

#include <algorithm>
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

    // Check if current move is en passant capture and then check if move allows
    // for en passant capture in the next move
    if (to == enPassantSquare) {
      // Move is an en passant capture
      setPieceAt({from.rank(), to.file()}, Piece(PieceType::None));
      move.type = MoveType::EnPassantCapture;
    }
    const auto rankDistance = std::abs(static_cast<long int>(from.rank()) -
                                       static_cast<long int>(to.rank()));
    if (piece.type == PieceType::Pawn && rankDistance == 2) {
      if (activeColor == PieceColor::White) {
        enPassantSquare = to.below();
      } else {
        enPassantSquare = to.above();
      }
    } else {
      enPassantSquare = {};
    }

    if (getPieceAt(to).type != PieceType::None) move.type = MoveType::Capture;

    setPieceAt(from, Piece(PieceType::None));
    setPieceAt(to, piece);

    generateOpponentLegalMoves();

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

    std::vector<Coordinate> attackedSquares;
    for (const auto& move : opponentLegalMoves)
      attackedSquares.push_back(move.to);

    computeLegalMoves(legalMoves, attackedSquares, square, piece);
  }
}
void Board2DArray::generateOpponentLegalMoves() {
  opponentLegalMoves.clear();
  for (size_t square = 0; square < 64; ++square) {
    const auto& piece = getPieceAt(square);

    if (piece.type == PieceType::None || piece.color != activeColor) continue;

    computeLegalMoves(opponentLegalMoves, {}, square, piece);
  }
}

void Board2DArray::computeLegalMoves(MoveSet& moveSet,
                                     std::vector<Coordinate> attackedSquares,
                                     const Coordinate& square,
                                     const Piece& piece) {
  switch (piece.type) {
    case PieceType::Pawn: {
      const auto pawnMoves = generatePawnMoves(square, piece);
      moveSet.insert(std::begin(pawnMoves), std::end(pawnMoves));
      break;
    }

    case PieceType::Knight: {
      const auto knightMoves = generateKnightMoves(square, piece);
      moveSet.insert(std::begin(knightMoves), std::end(knightMoves));
      break;
    }

    case PieceType::Bishop: {
      const auto bishopMoves = generateBishopMoves(square, piece);
      moveSet.insert(std::begin(bishopMoves), std::end(bishopMoves));
      break;
    }

    case PieceType::Rook: {
      const auto rookMoves = generateRookMoves(square, piece);
      moveSet.insert(std::begin(rookMoves), std::end(rookMoves));
      break;
    }

    case PieceType::Queen: {
      // Queen moves are the union of bishop-type and rook-type moves
      const auto queenMovesHorizVert = generateRookMoves(square, piece);
      const auto queenMovesDiag = generateBishopMoves(square, piece);
      moveSet.insert(std::begin(queenMovesHorizVert),
                     std::end(queenMovesHorizVert));
      moveSet.insert(std::begin(queenMovesDiag), std::end(queenMovesDiag));
      break;
    }

    case PieceType::King: {
      auto kingMoves = generateKingMoves(square, piece);

      std::cout << "Attacked Squares: ";
      for (const auto& atSquare : attackedSquares) {
        std::cout << atSquare.toAlgebraic() << "  ";
      }
      std::cout << "\n";

      std::erase_if(kingMoves, [attackedSquares](const auto& move) -> bool {
        return (std::find(attackedSquares.begin(), attackedSquares.end(),
                          move.to)) != attackedSquares.end();
      });

      moveSet.insert(std::begin(kingMoves), std::end(kingMoves));

      break;
    }

    case PieceType::None:
    default:
      break;
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

  // En passant capture
  if (enPassantSquare) {
    if (enPassantSquare.value() == captureLeftSquare) {
      const Move move(square, captureLeftSquare);
      legalPawnMoves.insert(move);
    } else if (enPassantSquare.value() == captureRightSquare) {
      const Move move(square, captureRightSquare);
      legalPawnMoves.insert(move);
    }
  }

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

Board2DArray::MoveSet Board2DArray::generateBishopMoves(
    const Coordinate& square, const Piece& piece) const {
  MoveSet bishopMoves;

  /* bishop moves horizontally or vertically until it hits a piece of the same
   * color, the end of the board or a piece of the opposite colour that it can
   * capture
   */

  {
    // first check up and left direction
    auto possibleSquare = square.above().left();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      bishopMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.above().left();
    }
  }

  {
    // first check up and right direction
    auto possibleSquare = square.above().right();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      bishopMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.above().right();
    }
  }
  {
    // first check down and left direction
    auto possibleSquare = square.below().left();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      bishopMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.below().left();
    }
  }
  {
    // first check down and right direction
    auto possibleSquare = square.below().right();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      bishopMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.below().right();
    }
  }

  return bishopMoves;
}

Board2DArray::MoveSet Board2DArray::generateRookMoves(
    const Coordinate& square, const Piece& piece) const {
  MoveSet rookMoves;

  /* rook moves horizontally or vertically until it hits a piece of the same
   * color, the end of the board or a piece of the opposite colour that it can
   * capture
   */

  {
    // first check up
    auto possibleSquare = square.above();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      rookMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.above();
    }
  }

  {
    // first check left
    auto possibleSquare = square.left();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      rookMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.left();
    }
  }
  {
    // first check down
    auto possibleSquare = square.below();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      rookMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.below();
    }
  }
  {
    // first check right direction
    auto possibleSquare = square.right();
    while (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const auto targetPiece = getPieceAt(possibleSquare);
      auto move = Move{square, possibleSquare};
      rookMoves.insert(move);

      if (targetPiece.type != PieceType::None) move.type = MoveType::Capture;

      possibleSquare = possibleSquare.right();
    }
  }

  return rookMoves;
}

Board2DArray::MoveSet Board2DArray::generateKingMoves(
    const Coordinate& square, const Piece& piece) const {
  MoveSet kingMoves;

  const std::vector<Coordinate> directions{
      square.above(),        square.above().left(), square.above().right(),
      square.right(),        square.left(),         square.below(),
      square.below().left(), square.below().right()};

  for (const auto& possibleSquare : directions) {
    if (!squareBlockedByOwnPieceOrOutsideBoard(possibleSquare, piece)) {
      const Move move{square, possibleSquare};
      kingMoves.insert(move);
    }
  }

  return kingMoves;
}

bool Board2DArray::squareBlockedByOwnPieceOrOutsideBoard(
    const Coordinate& targetSquare, const Piece& piece) const {
  const auto targetPiece = getPieceAt(targetSquare);

  if (targetSquare.isOutsideOfBoard()) {
    return true;
  }

  if (targetPiece.type != PieceType::None && targetPiece.color == piece.color)
    return true;

  return false;
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
