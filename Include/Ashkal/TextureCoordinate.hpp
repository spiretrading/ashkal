#ifndef ASHKAL_TEXTURE_COORDINATE_HPP
#define ASHKAL_TEXTURE_COORDINATE_HPP
#include <ostream>

namespace Ashkal {

  /**
   * Stores the U (horizontal) and V (vertical) coordinates for sampling
   * textures.
   */
  struct TextureCoordinate {

    /** The horizontal component (U) of the texture coordinate. */
    float m_u;

    /** The vertical component (V) of the texture coordinate. */
    float m_v;
  };

  inline std::ostream& operator <<(
      std::ostream& out, const TextureCoordinate& uv) {
    return out << "TextureCoordinate(" << uv.m_u << ", " << uv.m_v << ")";
  }
}

#endif
