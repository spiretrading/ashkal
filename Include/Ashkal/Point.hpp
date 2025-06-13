#ifndef ASHKAL_POINT_HPP
#define ASHKAL_POINT_HPP
#include <ostream>

namespace Ashkal {

  /** Stores a point in voxel space. */
  struct Point {

    /** The point's x-coordinate. */
    float m_x;

    /** The point's y-coordinate. */
    float m_y;

    /** The point's z-coordinate. */
    float m_z;

    friend auto operator <=>(const Point&, const Point&) = default;
  };

  inline std::ostream& operator <<(std::ostream& out, Point point) {
    return out <<
      "Point(" << point.m_x << ", " << point.m_y << ", " << point.m_z << ')';
  }
}

#endif
