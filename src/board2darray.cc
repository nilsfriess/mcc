#include "mcc/board2darray.hh"

#include <iostream>
#include <sstream>
#include <string>

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
        for (size_t i = 0; i < (size_t)squaresToSkip; ++i, ++file)
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

}  // namespace mcc
