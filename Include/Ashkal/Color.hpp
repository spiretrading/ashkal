#ifndef ASHKAL_COLOR_HPP
#define ASHKAL_COLOR_HPP
#include <cstdint>
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"

namespace Ashkal {

  /**
   * Represents an RGBA color packed into a 32-bit unsigned integer.
   */
  class Color {
    public:

      /**
       * Constructs an opaque black color (0, 0, 0, 255).
       */
      Color() noexcept;

      /**
       * Constructs an opaque color with the given red, green, blue components.
       * @param red Red component [0..255]
       * @param green Green component [0..255]
       * @param blue Blue component [0..255]
       */
      Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept;

      /**
       * Constructs a color with the given red, green, blue, and alpha
       * components.
       * @param red Red component [0..255]
       * @param green Green component [0..255]
       * @param blue Blue component [0..255]
       * @param alpha Alpha component [0..255]
       */
      Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
        std::uint8_t alpha) noexcept;

      /**
       * Constructs a color from an RGBA value.
       * @param rgba The integer representation of the Color in RGBA format.
       */
      explicit Color(std::uint32_t rgba) noexcept;

      /** Retrieves the red component (0..255). */
      std::uint8_t get_red() const noexcept;

      /** Sets the red component (0..255). */
      void set_red(std::uint8_t red) noexcept;

      /** Retrieves the green component (0..255). */
      std::uint8_t get_green() const noexcept;

      /** Sets the green component (0..255). */
      void set_green(std::uint8_t green) noexcept;

      /** Retrieves the blue component (0..255). */
      std::uint8_t get_blue() const noexcept;

      /** Sets the blue component (0..255). */
      void set_blue(std::uint8_t blue) noexcept;

      /** Retrieves the alpha component (0..255). */
      std::uint8_t get_alpha() const noexcept;

      /** Sets the alpha component (0..255). */
      void set_alpha(std::uint8_t alpha) noexcept;

      /** Returns the packed 0xRRGGBBAA value. */
      std::uint32_t as_rgba() const noexcept;

    private:
      std::uint32_t m_rgba;
  };

  /**
   * Adds two colors, channel-wise with saturation up to 255.
   * @param left  The left-hand color.
   * @param right The right-hand color.
   * @return A new Color where each RGB channel is added component wise.
   */
  inline Color operator +(Color left, Color right) {
    return Color(std::min<int>(left.get_red() + right.get_red(), 255),
      std::min<int>(left.get_green() + right.get_green(), 255),
      std::min<int>(left.get_blue() + right.get_blue(), 255), left.get_alpha());
  }

  inline std::ostream& operator <<(std::ostream& out, Color color) {
    return out << "Color(" << static_cast<int>(color.get_red()) << ", " <<
      static_cast<int>(color.get_green()) << ", " <<
      static_cast<int>(color.get_blue()) << ", " <<
      static_cast<int>(color.get_alpha()) << ')';
  }

  inline Color::Color() noexcept
    : m_rgba(0x000000FFu) {}

  inline Color::Color(
    std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
    : Color(red, green, blue, 255) {}

  inline Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
    std::uint8_t alpha) noexcept
    : m_rgba((std::uint32_t(red) << 24) | (std::uint32_t(green) << 16) |
        (std::uint32_t(blue) <<  8) | std::uint32_t(alpha)) {}

  inline Color::Color(std::uint32_t rgba) noexcept
    : m_rgba(rgba) {}

  inline std::uint8_t Color::get_red() const noexcept {
    return std::uint8_t((m_rgba >> 24) & 0xFFu);
  }

  inline void Color::set_red(std::uint8_t red) noexcept {
    m_rgba = (m_rgba & 0x00FFFFFFu) | (std::uint32_t(red) << 24);
  }

  inline std::uint8_t Color::get_green() const noexcept {
    return std::uint8_t((m_rgba >> 16) & 0xFFu);
  }

  inline void Color::set_green(std::uint8_t green) noexcept {
    m_rgba = (m_rgba & 0xFF00FFFFu) | (std::uint32_t(green) << 16);
  }

  inline std::uint8_t Color::get_blue() const noexcept {
    return std::uint8_t((m_rgba >> 8) & 0xFFu);
  }

  inline void Color::set_blue(std::uint8_t blue) noexcept {
    m_rgba = (m_rgba & 0xFFFF00FFu) | (std::uint32_t(blue) << 8);
  }

  inline std::uint8_t Color::get_alpha() const noexcept {
    return std::uint8_t(m_rgba & 0xFFu);
  }

  inline void Color::set_alpha(std::uint8_t alpha) noexcept {
    m_rgba = (m_rgba & 0xFFFFFF00u) | std::uint32_t(alpha);
  }

  inline std::uint32_t Color::as_rgba() const noexcept {
    return m_rgba;
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

#endif
