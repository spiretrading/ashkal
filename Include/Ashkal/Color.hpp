#ifndef ASHKAL_COLOR_HPP
#define ASHKAL_COLOR_HPP
#include <cstdint>
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
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

    /** The color's alpha/transparency component. */
    std::uint8_t m_alpha;

    friend auto operator <=>(const Color&, const Color&) = default;
  };

  inline Color operator +(Color left, Color right) {
    return Color(std::min<int>(left.m_red + right.m_red, 255),
      std::min<int>(left.m_green + right.m_green, 255),
      std::min<int>(left.m_blue + right.m_blue, 255), left.m_alpha);
  }

  inline std::ostream& operator <<(std::ostream& out, Color color) {
    return out << "Color(" << static_cast<int>(color.m_red) << ", " <<
      static_cast<int>(color.m_green) << ", " <<
      static_cast<int>(color.m_blue) << ", " <<
      static_cast<int>(color.m_alpha) << ')';
  }

  inline std::string COLOR_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Color make_color(unsigned char red, unsigned green, unsigned char blue,
        unsigned char alpha) {
      Color color;
      color.m_red = red;
      color.m_green = green;
      color.m_blue = blue;
      color.m_alpha = alpha;
      return color;
    }

    Color add_color(Color left, Color right) {
      return make_color(min((int)(left.m_red) + right.m_red, 255),
        min((int)(left.m_green) + right.m_green, 255),
        min((int)(left.m_blue) + right.m_blue, 255), left.m_alpha);
    }

    bool is_equal_color(Color left, Color right) {
      return left.m_red == right.m_red && left.m_green == right.m_green &&
        left.m_blue == right.m_blue && left.m_alpha == right.m_alpha;
    }

    void print_color(Color color) {
      printf("Color(%hhu, %hhu, %hhu, %hhu)",
        color.m_red, color.m_green, color.m_blue, color.m_alpha);
    }
  );
}

BOOST_COMPUTE_ADAPT_STRUCT(
  Ashkal::Color, Color, (m_red, m_green, m_blue, m_alpha));

#endif
