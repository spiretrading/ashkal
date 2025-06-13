#ifndef ASHKAL_AMBIENT_LIGHT_HPP
#define ASHKAL_AMBIENT_LIGHT_HPP
#include <ostream>
#include "Ashkal/ShadingTerm.hpp"

namespace Ashkal {

  /**
   * Represents an ambient light: uniform, non-directional illumination applied
   * equally across all surfaces.
   */
  struct AmbientLight {

    /* RGB color of the ambient light in linear color space. */
    Color m_color;

    /**
     * Scalar intensity multiplier for brightness (0.0 means no contribution).
     */
    float m_intensity;
  };

  /**
   * Computes the shading term for a given AmbientLight.
   * @param light The ambient light to sample from.
   * @return A ShadingTerm encapsulating the light's color and intensity.
   */
  inline ShadingTerm calculate_shading(const AmbientLight& light) {
    return ShadingTerm(light.m_color, light.m_intensity);
  }

  inline std::ostream& operator<<(std::ostream& out,
      const AmbientLight& light) {
    return out << "AmbientLight(" << light.m_color << ", " <<
      light.m_intensity << ')';
  }
}

#endif
