#ifndef ASHKAL_VERTEX_TRIANGLE_HPP
#define ASHKAL_VERTEX_TRIANGLE_HPP
#include <ostream>

namespace Ashkal {

  /**
   * Stores the indices of the vertices making up a triangle. Vertices are
   * ordered counter-clockwise.
   */
  struct VertexTriangle {

    /** Index of the first vertex. */
    int m_a;

    /** Index of the second vertex. */
    int m_b;

    /** Index of the third vertex. */
    int m_c;
  };

  inline std::ostream& operator <<(
      std::ostream& out, const VertexTriangle& triangle) {
    return out << "VertexTriangle(" << triangle.m_a << ", " << triangle.m_b <<
      ", " << triangle.m_c << ')';
  }
}

#endif
