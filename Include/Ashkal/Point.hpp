#ifndef ASHKAL_POINT_HPP
#define ASHKAL_POINT_HPP
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
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

  /** Returns a point whose coordinate's are all floored. */
  inline Point floor(Point point) {
    return Point(
      std::floor(point.m_x), std::floor(point.m_y), std::floor(point.m_z));
  }

  /** Tests if a point is contained within a bounding box. */
  inline bool contains(Point start, Point end, Point point) {
    return point.m_x >= start.m_x && point.m_x < end.m_x &&
      point.m_y >= start.m_y && point.m_y < end.m_y &&
      point.m_z >= start.m_z && point.m_z < end.m_z;
  }

  inline std::ostream& operator <<(std::ostream& out, Point point) {
    return out <<
      "Point(" << point.m_x << ", " << point.m_y << ", " << point.m_z << ')';
  }

  inline std::string POINT_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Point make_point(float x, float y, float z) {
      Point point;
      point.m_x = x;
      point.m_y = y;
      point.m_z = z;
      return point;
    }

    Point floor_point(Point point) {
      return make_point(floor(point.m_x), floor(point.m_y), floor(point.m_z));
    }

    bool contains(Point start, Point end, Point point) {
      return point.m_x >= start.m_x && point.m_x < end.m_x &&
        point.m_y >= start.m_y && point.m_y < end.m_y &&
        point.m_z >= start.m_z && point.m_z < end.m_z;
    }

    void print_point(Point point) {
      printf("Point(%f, %f, %f)", point.m_x, point.m_y, point.m_z);
    }
  );
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::Point, Point, (m_x, m_y, m_z));

#endif
