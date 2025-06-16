#ifndef ASHKAL_PLANE_HPP
#define ASHKAL_PLANE_HPP
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Stores a plane in 3D space defined by a normal vector and offset. */
  struct Plane {

    /** The normalized normal vector perpendicular to the plane. */
    Vector m_normal;

    /** The offset (signed distance) from the origin along the normal. */
    float m_d;
  };

  /**
   * Computes the signed distance from a point to a plane.
   * @param plane The plane to test against.
   * @param point The point whose distance to the plane is calculated.
   * @return Positive if the point lies in the direction of the normal
   *         (in front of the plane), negative if behind, or zero if exactly on
   *         the plane.
   */
  inline float distance(const Plane& plane, const Point& point) {
    return dot(plane.m_normal, Vector(point)) + plane.m_d;
  }
}

#endif
