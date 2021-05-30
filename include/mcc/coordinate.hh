#pragma once

#include <cstddef>
#include <iostream>
#include <string>

namespace mcc {
struct Coordinate {
 private:
  std::size_t _rank = 0;
  std::size_t _file = 0;

  bool outsideOfBoard = false;

 public:
  Coordinate() = default;

  Coordinate(std::size_t t_rank, std::size_t t_file)
      : _rank(t_rank), _file(t_file) {}

  Coordinate(std::size_t t_position)
      : _rank(t_position / 8), _file(t_position % 8) {}

  Coordinate(const std::string& t_algebraic) {
    const auto file = static_cast<std::size_t>(t_algebraic.at(0) - 'a');
    const auto rank = static_cast<std::size_t>(t_algebraic.at(1) - '0');

    // we number from top to bottom, algebraic is from bottom to top
    _rank = 8 - rank;
    _file = file;
  }

  bool isOutsideOfBoard() const { return outsideOfBoard; }

  std::size_t rank() const { return _rank; }
  std::size_t file() const { return _file; }

  void setRank(const size_t& t_rank) {
    if (t_rank < 8) _rank = t_rank;
  }
  void setFile(const size_t& t_file) {
    if (t_file < 8) _file = t_file;
  }

  std::size_t to64Position() const { return 8 * _rank + _file; }
  std::string toAlgebraic() const {
    return std::string(1, 'a' + static_cast<char>(_file)) +
           std::to_string(8 - _rank);
  }

  bool operator==(const Coordinate& other) const {
    return _rank == other._rank && _file == other._file;
  }

  Coordinate above() const {
    if (outsideOfBoard) return *this;

    if (_rank != 0)
      return Coordinate(_rank - 1, _file);
    else {
      Coordinate coord = *this;
      coord.outsideOfBoard = true;
      return coord;
    }
  }

  Coordinate below() const {
    if (outsideOfBoard) return *this;

    if (_rank != 7)
      return Coordinate(_rank + 1, _file);
    else {
      Coordinate coord = *this;
      coord.outsideOfBoard = true;
      return coord;
    }
  }
  Coordinate left() const {
    if (outsideOfBoard) return *this;

    if (_file != 0)
      return Coordinate(_rank, _file - 1);
    else {
      Coordinate coord = *this;
      coord.outsideOfBoard = true;
      return coord;
    }
  }
  Coordinate right() const {
    if (outsideOfBoard) return *this;

    if (_file != 7)
      return Coordinate(_rank, _file + 1);
    else {
      Coordinate coord = *this;
      coord.outsideOfBoard = true;
      return coord;
    }
  }
};

struct coord_hash {
  std::size_t operator()(const Coordinate& coord) const {
    const auto h1 = std::hash<size_t>{}(coord.rank());
    const auto h2 = std::hash<size_t>{}(coord.file());

    return h1 ^ (h2 << 1);
  }
};

}  // namespace mcc