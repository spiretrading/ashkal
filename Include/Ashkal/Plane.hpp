#ifndef ASHKAL_PLANE_HPP
#define ASHKAL_PLANE_HPP
#include "Ashkal/Point.hpp"
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
   * Constructs a Plane through three non-collinear points.
   * @param a The first point on the plane.
   * @param b The second point on the plane.
   * @param c The third point on the plane.
   * @return A Plane object whose normal is the normalized cross(v, u) and
   *         whose offset d satisfies dot(n, a) + d = 0.
   */
  inline Plane make_plane(const Point& a, const Point& b, const Point& c) {
    auto u = b - a;
    auto v = c - a;
    auto n = normalize(cross(v, u));
    auto d = -dot(n, Vector(a));
    return Plane(n, d);
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

  /**
   * Determines whether a point lies on the same side as a plane's normal.
   * @param plane The plane against which to test.
   * @param point The point to classify.
   * @return true iff point is on or in front of the plane.
   */
  inline bool is_in_front(const Plane& plane, const Point& point) {
    return distance(plane, point) >= 0;
  }
}

#endif
