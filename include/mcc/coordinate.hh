#pragma once

#include <cstddef>

namespace mcc {
struct Coordinate {
 private:
  std::size_t _rank = 0;
  std::size_t _file = 0;

 public:
  std::size_t rank() const { return _rank; }
  std::size_t file() const { return _file; }

  void setRank(const size_t& t_rank) { _rank = t_rank; }
  void setFile(const size_t& t_file) { _file = t_file; }
};
}  // namespace mcc