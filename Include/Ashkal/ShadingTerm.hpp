#ifndef ASHKAL_SHADING_TERM_HPP
#define ASHKAL_SHADING_TERM_HPP
#include <algorithm>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  struct ShadingTerm {
    Color m_color;
    float m_intensity;
  };

  ShadingTerm operator +(const ShadingTerm& left, const ShadingTerm& right) {
    return ShadingTerm(
      left.m_color + right.m_color, left.m_intensity + right.m_intensity);
  }

  inline Color apply(const ShadingTerm& term, Color color) {
    auto r =
      (term.m_color.get_red() * color.get_red() * term.m_intensity) / 255;
    auto g =
      (term.m_color.get_green() * color.get_green() * term.m_intensity) / 255;
    auto b =
      (term.m_color.get_blue() * color.get_blue() * term.m_intensity) / 255;
    return Color(r, g, b, color.get_alpha());
  }
}

#endif
