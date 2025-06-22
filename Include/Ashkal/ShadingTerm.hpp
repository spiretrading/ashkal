#ifndef ASHKAL_SHADING_TERM_HPP
#define ASHKAL_SHADING_TERM_HPP
#include "Ashkal/Color.hpp"

namespace Ashkal {

  /**
   * Encapsulates the color of lighting and a scalar intensity factor, used to
   * modulate base material colors in shading calculations.
   */
  struct ShadingTerm {

    /** The color component of the shading term. */
    Color m_color;

    /** The intensity component of the shading term. */
    float m_intensity;
  };

  /**
   * Combines the colors and intensities of two terms by adding each component.
   * @param left The first shading term.
   * @param right The second shading term.
   * @return A new ShadingTerm with summed color and intensity.
   */
  inline ShadingTerm operator +(
      const ShadingTerm& left, const ShadingTerm& right) {
    return ShadingTerm(
      left.m_color + right.m_color, left.m_intensity + right.m_intensity);
  }

  /**
   * Applies a shading term to a base color.
   * @param term The shading term to apply.
   * @param color The base Color to be shaded.
   * @return The resulting Color after shading.
   */
  inline Color apply(const ShadingTerm& term, Color color) {
    auto r =
      (term.m_color.get_red() * color.get_red() * term.m_intensity) / 255;
    auto g =
      (term.m_color.get_green() * color.get_green() * term.m_intensity) / 255;
    auto b =
      (term.m_color.get_blue() * color.get_blue() * term.m_intensity) / 255;
    return Color(r, g, b, color.get_alpha());
  }

  inline std::ostream& operator<<(std::ostream& out, const ShadingTerm& term) {
    return out << "ShadingTerm(" << term.m_color << ", " << term.m_intensity <<
      ')';
  }
}

#endif
