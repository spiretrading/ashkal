#ifndef ASHKAL_SOLID_COLOR_SAMPLER_HPP
#define ASHKAL_SOLID_COLOR_SAMPLER_HPP
#include "Ashkal/ColorSampler.hpp"

namespace Ashkal {

  /** A ColorSampler that always returns a constant color. */
  class SolidColorSampler final : public ColorSampler {
    public:

      /**
       * Constructs the sampler with a solid color.
       * @param color The color to return on every sample.
       */
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
