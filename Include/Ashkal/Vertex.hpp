#ifndef ASHKAL_VERTEX_HPP
#define ASHKAL_VERTEX_HPP
#include <ostream>
#include "Ashkal/Point.hpp"
#include "Ashkal/TextureCoordinate.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Stores the information associated with a vertex. */
  struct Vertex {

    /** Position of the vertex. */
    Point m_position;

    /** Texture coordinate for sampling textures. */
    TextureCoordinate m_uv;

    /** Normal vector at the vertex. */
    Vector m_normal;
  };

  inline std::ostream& operator<<(std::ostream& out, const Vertex& vertex) {
    return out << "Vertex(" << vertex.m_position << ", " << vertex.m_uv <<
      ", " << vertex.m_normal << ')';
  }
}

#endif
