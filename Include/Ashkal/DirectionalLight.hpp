#ifndef ASHKAL_DIRECTIONAL_LIGHT_HPP
#define ASHKAL_DIRECTIONAL_LIGHT_HPP
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  struct DirectionalLight {
    Vector m_direction;
    Color m_color;
    float m_intensity;
  };

  inline std::string DIRECTIONAL_LIGHT_CL_SOURCE =
    BOOST_COMPUTE_STRINGIZE_SOURCE(
      Color apply_directional_light(
          DirectionalLight light, Vector surface_normal, Color color) {
        float shading = calculate_shading(surface_normal, light.m_direction);
        return apply_shading(color, light.m_color, shading, light.m_intensity);
      });
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::DirectionalLight,
  DirectionalLight, (m_direction, m_color, m_intensity));

#endif
