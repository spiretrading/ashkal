#ifndef ASHKAL_BOUNDING_BOX_HPP
#define ASHKAL_BOUNDING_BOX_HPP
#include <cmath>
#include <ostream>
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /**
   * Represents an axis-aligned box defined by minimum and maximum corner
   * points.
   */
  class BoundingBox {
    public:

      /** Constructs a unit cube centered at the origin. */
      BoundingBox();

      /**
       * Constructs a BoundingBox along two corners.
       * @param minimum The minimum corner.
       * @param maximum The maximum corner.
       */
      BoundingBox(const Point& minimum, const Point& maximum);

      /** Returns the minimum (corner) point of the bounding box. */
      const Point& get_minimum() const;

      /** Returns the maximum (corner) point of the bounding box. */
      const Point& get_maximum() const;

      /**
       * Applies an affine transformation to the bounding box and updates its
       * axis-aligned bounds.
       * @param transformation The matrix representing the affine transform.
       */
      void apply(const Matrix& transformation);

    private:
      Point m_minimum;
      Point m_maximum;
  };

  /**
   * Computes the bounding box that encloses two input boxes.
   * @param a The first bounding box.
   * @param b The second bounding box.
   * @return A BoundingBox that fully contains both input boxes.
   */
  inline BoundingBox merge(const BoundingBox& a, const BoundingBox& b) {
    auto min_x = std::min(a.get_minimum().m_x, b.get_minimum().m_x);
    auto min_y = std::min(a.get_minimum().m_y, b.get_minimum().m_y);
    auto min_z = std::min(a.get_minimum().m_z, b.get_minimum().m_z);
    auto max_x = std::max(a.get_maximum().m_x, b.get_maximum().m_x);
    auto max_y = std::max(a.get_maximum().m_y, b.get_maximum().m_y);
    auto max_z = std::max(a.get_maximum().m_z, b.get_maximum().m_z);
    return BoundingBox(Point(min_x, min_y, min_z), Point(max_x, max_y, max_z));
  }

  /**
   * Tests whether two bounding boxes overlap.
   * @param a First bounding box.
   * @param b Second bounding box.
   * @return True iff boxes intersect on all three axes.
   */
  inline bool intersects(const BoundingBox& a, const BoundingBox& b) {
    return a.get_minimum().m_x <= b.get_maximum().m_x &&
      a.get_maximum().m_x >= b.get_minimum().m_x &&
      a.get_minimum().m_y <= b.get_maximum().m_y &&
      a.get_maximum().m_y >= b.get_minimum().m_y &&
      a.get_minimum().m_z <= b.get_maximum().m_z &&
      a.get_maximum().m_z >= b.get_minimum().m_z;
  }

  /**
   * Tests if a point lies within or on the boundary of a bounding box.
   * @param box The bounding box to test.
   * @param point The point to check.
   * @return True iff the point is inside or on the box.
   */
  inline bool contains(const BoundingBox& box, const Point& point) {
    return point.m_x >= box.get_minimum().m_x &&
      point.m_x <= box.get_maximum().m_x &&
      point.m_y >= box.get_minimum().m_y &&
      point.m_y <= box.get_maximum().m_y &&
      point.m_z >= box.get_minimum().m_z && point.m_z <= box.get_maximum().m_z;
  }

  inline std::ostream& operator <<(std::ostream& out, const BoundingBox& box) {
    return out << "BoundingBox(" << box.get_minimum() << ", " <<
      box.get_maximum() << ')';
  }

  inline BoundingBox::BoundingBox()
    : BoundingBox(Point(-0.5f, -0.5f, -0.5f), Point(0.5f, 0.5f, 0.5f)) {}

  inline BoundingBox::BoundingBox(const Point& minimum, const Point& maximum)
    : m_minimum(minimum),
      m_maximum(maximum) {}

  const Point& BoundingBox::get_minimum() const {
    return m_minimum;
  }

  const Point& BoundingBox::get_maximum() const {
    return m_maximum;
  }

  void BoundingBox::apply(const Matrix& transformation) {
    auto center = Point((m_minimum.m_x + m_maximum.m_x) * 0.5f,
      (m_minimum.m_y + m_maximum.m_y) * 0.5f,
      (m_minimum.m_z + m_maximum.m_z) * 0.5f);
    auto half_point = Vector((m_maximum.m_x - m_minimum.m_x) * 0.5f,
      (m_maximum.m_y - m_minimum.m_y) * 0.5f,
      (m_maximum.m_z - m_minimum.m_z) * 0.5f);
    auto new_center = transformation * center;
    auto vx = transformation * Vector(half_point.m_x, 0, 0);
    auto vy = transformation * Vector(0, half_point.m_y, 0);
    auto vz = transformation * Vector(0, 0, half_point.m_z);
    auto new_half_point =
      Vector(std::abs(vx.m_x) + std::abs(vy.m_x) + std::abs(vz.m_x),
        std::abs(vx.m_y) + std::abs(vy.m_y) + std::abs(vz.m_y),
        std::abs(vx.m_z) + std::abs(vy.m_z) + std::abs(vz.m_z));
    m_minimum.m_x = new_center.m_x - new_half_point.m_x;
    m_minimum.m_y = new_center.m_y - new_half_point.m_y;
    m_minimum.m_z = new_center.m_z - new_half_point.m_z;
    m_maximum.m_x = new_center.m_x + new_half_point.m_x;
    m_maximum.m_y = new_center.m_y + new_half_point.m_y;
    m_maximum.m_z = new_center.m_z + new_half_point.m_z;
  }
}

#endif
