#pragma once

#include <ostream>
    
namespace mcc {
enum Colour { White = 0, Black = 1 };
inline Colour get_other_colour(Colour colour) {
  return (colour == Colour::White) ? Colour::Black : Colour::White;
}
} // namespace mcc

inline std::ostream &operator<<(std::ostream &out, mcc::Colour colour) {
  out << ((colour == mcc::Colour::White) ? "White" : "Black");
  return out;
}
