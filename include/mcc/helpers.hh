#pragma once

namespace mcc {
static wchar_t fenToUnicode(char fen) {
  switch (fen) {
    case 'p':
      return L'♟';
    case 'P':
      return L'♙';

    case 'b':
      return L'♝';
    case 'B':
      return L'♗';

    case 'n':
      return L'♞';
    case 'N':
      return L'♘';

    case 'r':
      return L'♜';
    case 'R':
      return L'♖';

    case 'q':
      return L'♛';
    case 'Q':
      return L'♕';

    case 'k':
      return L'♚';
    case 'K':
      return L'♔';
  }
  return ' ';
}
}  // namespace mcc