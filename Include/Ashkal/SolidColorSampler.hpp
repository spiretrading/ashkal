#ifndef ASHKAL_SOLID_COLOR_SAMPLER_HPP
#define ASHKAL_SOLID_COLOR_SAMPLER_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/ColorSampler.hpp"

namespace Ashkal {
  class SolidColorSampler final : public ColorSampler {
    public:
      explicit SolidColorSampler(Color color);

      Color sample(const TextureCoordinate& uv) const override;

    private:
      Color m_color;
  };

  inline SolidColorSampler::SolidColorSampler(Color color)
    : m_color(color) {}

  inline Color SolidColorSampler::sample(const TextureCoordinate& uv) const {
    return m_color;
  }
}

#endif
