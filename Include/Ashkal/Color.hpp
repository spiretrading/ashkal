#ifndef ASHKAL_COLOR_HPP
#define ASHKAL_COLOR_HPP
#include <cstdint>
#include <ostream>

namespace Ashkal {

  /** Represents an RGBA color packed into a 32-bit unsigned integer. */
  class Color {
    public:

      /** Constructs an opaque black color (0, 0, 0, 255). */
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

      bool operator ==(const Color&) const = default;

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

  /**
   * Linearly interpolates between two Color objects.
   * @param left The color corresponding to t = 0.
   * @param right The color corresponding to t = 1.
   * @param t Interpolation parameter in the range [0, 1].
   * @return The interpolated Color.
   */
  inline Color lerp(Color left, Color right, float t) {
    auto red = static_cast<std::uint8_t>(std::lerp(
      static_cast<float>(left.get_red()),
      static_cast<float>(right.get_red()), t));
    auto green = static_cast<std::uint8_t>(std::lerp(
      static_cast<float>(left.get_green()),
      static_cast<float>(right.get_green()), t));
    auto blue = static_cast<std::uint8_t>(std::lerp(
      static_cast<float>(left.get_blue()),
      static_cast<float>(right.get_blue()), t));
    auto alpha = static_cast<std::uint8_t>(std::lerp(
      static_cast<float>(left.get_alpha()),
      static_cast<float>(right.get_alpha()), t));
    return Color(red, green, blue, alpha);
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
}

#endif
