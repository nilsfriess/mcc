#pragma once

#include <cstddef>
#include <string>

namespace mcc {
struct Coordinate {
 private:
  std::size_t _rank = 0;
  std::size_t _file = 0;

 public:
  Coordinate(std::size_t t_rank, std::size_t t_file)
      : _rank(t_rank), _file(t_file) {}

  Coordinate(std::size_t t_position)
      : _rank(t_position / 8), _file(t_position % 8) {}

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
    return std::string(1, 'a' + static_cast<char>(_rank)) +
           std::to_string(_file + 1);
  }

  bool operator==(const Coordinate& other) const {
    return _rank == other._rank && _file == other._file;
  }

  Coordinate above() const {
    if (_rank != 0)
      return Coordinate(_rank - 1, _file);
    else
      return *this;
  }

  Coordinate below() const {
    if (_rank != 7)
      return Coordinate(_rank + 1, _file);
    else
      return *this;
  }
  Coordinate left() const {
    if (_file != 0)
      return Coordinate(_rank, _file - 1);
    else
      return *this;
  }
  Coordinate right() const {
    if (_file != 7)
      return Coordinate(_rank, _file + 1);
    else
      return *this;
  }
};
}  // namespace mcc