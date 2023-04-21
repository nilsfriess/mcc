#pragma once

#include "mcc/board.hh"
#include "mcc/move_generator.hh"

namespace mcc {

/* The actual engine class. */
class mcc {
  board m_board;
  move_generator m_generator;

public:
  mcc(std::string fen) : m_board{fen}, m_generator{&m_board} {}

  std::vector<move> generate_pseudo_legal() const {
    return m_generator.generate_pseudo_legal();
  }
};

}; // namespace mcc
