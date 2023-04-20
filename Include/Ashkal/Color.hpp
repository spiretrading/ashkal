#ifndef ASHKAL_COLOR_HPP
#define ASHKAL_COLOR_HPP
#include <cstdint>
#include <ostream>
#include "Ashkal/Ashkal.hpp"

namespace Ashkal {
  struct Color {
    std::uint8_t m_red;
    std::uint8_t m_green;
    std::uint8_t m_blue;
    std::uint8_t m_alpha;

    friend auto operator <=>(const Color&, const Color&) = default;
  };

  std::ostream& operator <<(std::ostream& out, Color color) {
    return out << "Color(" << static_cast<int>(color.m_red) << ", " <<
      static_cast<int>(color.m_green) << ", " <<
      static_cast<int>(color.m_blue) << ", " <<
      static_cast<int>(color.m_alpha) << ')';
  }
}

#endif
