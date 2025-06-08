#ifndef ASHKAL_COLOR_SAMPLER_HPP
#define ASHKAL_COLOR_SAMPLER_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/TextureCoordinate.hpp"

namespace Ashkal {
  class ColorSampler {
    public:
      virtual ~ColorSampler() = default;

      virtual Color sample(const TextureCoordinate& uv) const = 0;
  };
}

#endif
