#ifndef ASHKAL_VERTEX_HPP
#define ASHKAL_VERTEX_HPP
#include "Ashkal/Color.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/TextureCoordinate.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  struct Vertex {
    Point m_position;
    TextureCoordinate m_uv;
    Vector m_normal;
  };
}

#endif
