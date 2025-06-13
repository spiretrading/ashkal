#ifndef ASHKAL_COLOR_SAMPLER_HPP
#define ASHKAL_COLOR_SAMPLER_HPP
#include "Ashkal/Color.hpp"
#include "Ashkal/TextureCoordinate.hpp"

namespace Ashkal {

  /**
   * Abstract interface for color sampling over texture coordinates. Provides a
   * uniform API for texture mappers and procedural samplers to return a Color
   * given a (u, v) coordinate.
   */
  class ColorSampler {
    public:
      virtual ~ColorSampler() = default;

      /**
       * Sample the color at the specified texture coordinate.
       * @param uv The texture coordinate to sample.
       * @return The Color fetched or computed at that coordinate.
       */
      virtual Color sample(const TextureCoordinate& uv) const = 0;

    protected:
      ColorSampler() = default;

    private:
      ColorSampler(const ColorSampler&) = delete;
      ColorSampler& operator=(const ColorSampler&) = delete;
  };
}

#endif
