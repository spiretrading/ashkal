#ifndef ASHKAL_VERTEX_HPP
#define ASHKAL_VERTEX_HPP
#include "Ashkal/Color.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  struct Vertex {
    Point m_position;
    Vector m_normal;
    Color m_color;
  };
}

#endif
