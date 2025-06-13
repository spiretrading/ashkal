#ifndef ASHKAL_DIRECTIONAL_LIGHT_HPP
#define ASHKAL_DIRECTIONAL_LIGHT_HPP
#include <algorithm>
#include <ostream>
#include "Ashkal/ShadingTerm.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /**
   * Represents a directional light source with a uniform direction. Used to
   * simulate light coming from infinitely far away, with all rays parallel in
   * the given direction.
   */
  struct DirectionalLight {

    /** The direction toward the light source. Should be normalized. */
    Vector m_direction;

    /** The color of the light in linear space. */
    Color m_color;

    /** Scalar intensity multiplier for brightness. */
    float m_intensity;
  };

  /**
   * Computes the diffuse intensity between a surface normal and a light
   * direction using Lambert's cosine law.
   * @param normal The normalized surface normal vector.
   * @param direction The normalized direction toward the light.
   * @return A scalar in [0, 1] representing the intensity of the light source.
   */
  inline float calculate_intensity(
      const Vector& normal, const Vector& direction) {
    return std::max(dot(normal, direction), 0.f);
  }

  /**
   * Builds a ShadingTerm for a surface under a directional light.
   * @param light The directional light source to sample.
   * @param normal The normalized surface normal at the shading point.
   * @return A ShadingTerm encoding the color and computed intensity.
   */
  inline ShadingTerm calculate_shading(
      const DirectionalLight& light, const Vector& normal) {
    auto intensity = calculate_intensity(normal, -light.m_direction);
    return ShadingTerm(light.m_color, intensity);
  }

  inline std::ostream& operator<<(
      std::ostream& out, const DirectionalLight& light) {
    return out << "DirectionalLight(" << light.m_direction << ", " <<
      light.m_color << ", " << light.m_intensity << ')';
  }
}

#endif
