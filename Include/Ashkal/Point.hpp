#ifndef ASHKAL_POINT_HPP
#define ASHKAL_POINT_HPP
#include <ostream>
#include "Ashkal/Ashkal.hpp"

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

  inline Point floor(Point point) {
    return Point(
      std::floor(point.m_x), std::floor(point.m_y), std::floor(point.m_z));
  }

  inline bool contains(Point start, Point end, Point point) {
    return point.m_x >= start.m_x && point.m_x < end.m_x &&
      point.m_y >= start.m_y && point.m_y < end.m_y &&
      point.m_z >= start.m_z && point.m_z < end.m_z;
  }

  inline std::ostream& operator <<(std::ostream& out, Point point) {
    return out <<
      "Point(" << point.m_x << ", " << point.m_y << ", " << point.m_z << ')';
  }
}

#endif
