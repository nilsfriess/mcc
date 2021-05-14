#include "mcc/piece.hh"

#include <cctype>

namespace mcc {
Piece::Piece(char fen) {
  if (islower(fen))
    color = PieceColor::Black;
  else
    color = PieceColor::White;

  switch (fen) {
    case 'p':
    case 'P':
      type = PieceType::Pawn;
      break;

    case 'b':
    case 'B':
      type = PieceType::Bishop;
      break;

    case 'n':
    case 'N':
      type = PieceType::Knight;
      break;

    case 'r':
    case 'R':
      type = PieceType::Rook;
      break;

    case 'q':
    case 'Q':
      type = PieceType::Queen;
      break;

    case 'k':
    case 'K':
      type = PieceType::King;
      break;

    default:
      type = PieceType::None;
  }
}

char Piece::toFENChar() const {
  char fenChar;
  switch (type) {
    case PieceType::Pawn:
      fenChar = 'p';
      break;

    case PieceType::Bishop:
      fenChar = 'b';
      break;

    case PieceType::Knight:
      fenChar = 'n';
      break;

    case PieceType::Rook:
      fenChar = 'r';
      break;

    case PieceType::Queen:
      fenChar = 'q';
      break;

    case PieceType::King:
      fenChar = 'k';
      break;

    case PieceType::None:
    default:
      fenChar = ' ';
      break;
  }

  if (color == PieceColor::White) fenChar = static_cast<char>(toupper(fenChar));

  return fenChar;
}
}  // namespace mcc