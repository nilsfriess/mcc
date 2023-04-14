#pragma once

#include "mcc/board.hh"
#include "mcc/move_generator.hh"

namespace mcc {

/* The actual engine class. */
class mcc {
  board board;
  move_generator generator;
};

}; // namespace mcc
