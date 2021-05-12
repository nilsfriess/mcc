#pragma once

#include "mcc/board.hh"
#include "mcc/board2darray.hh"

namespace mcc {
struct MCC {
  Board<Board2DArray> board;
};
}  // namespace mcc