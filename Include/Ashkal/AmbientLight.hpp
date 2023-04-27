#ifndef ASHKAL_AMBIENT_LIGHT_HPP
#define ASHKAL_AMBIENT_LIGHT_HPP
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"

namespace Ashkal {
  struct AmbientLight {
    Color m_color;

    float m_intensity;
  };

  inline std::string AMBIENT_LIGHT_CL_SOURCE =
    BOOST_COMPUTE_STRINGIZE_SOURCE(
      Color apply_shading(
          Color source, Color light, float shading_factor, float intensity) {
        float r =
          (source.m_red * light.m_red * shading_factor * intensity) / 255.f;
        float g = (source.m_green *
          light.m_green * shading_factor * intensity) / 255.f;
        float b =
          (source.m_blue * light.m_blue * shading_factor * intensity) / 255.f;
        return make_color(r, g, b, source.m_alpha);
      }

      float calculate_shading(Vector normal, Vector light_direction) {
        return fabs(vector_dot(normal, light_direction));
      }

      Color apply_ambient_light(AmbientLight light, Color color) {
        return apply_shading(color, light.m_color, 1.f, light.m_intensity);
      }
    );
}

BOOST_COMPUTE_ADAPT_STRUCT(
  Ashkal::AmbientLight, AmbientLight, (m_color, m_intensity));

#endif
