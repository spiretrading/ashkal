#ifndef ASHKAL_COLOR_HPP
#define ASHKAL_COLOR_HPP
#include <cstdint>
#include <ostream>
#include "Ashkal/Ashkal.hpp"

namespace Ashkal {

  /** Stores a color. */
  struct Color {

    /** The color's red component. */
    std::uint8_t m_red;

    /** The color's green component. */
    std::uint8_t m_green;

    /** The color's blue component. */
    std::uint8_t m_blue;

    /** The color's alpha/transparancy component. */
    std::uint8_t m_alpha;

    friend auto operator <=>(const Color&, const Color&) = default;
  };

  inline std::ostream& operator <<(std::ostream& out, Color color) {
    return out << "Color(" << static_cast<int>(color.m_red) << ", " <<
      static_cast<int>(color.m_green) << ", " <<
      static_cast<int>(color.m_blue) << ", " <<
      static_cast<int>(color.m_alpha) << ')';
  }
}

#endif
