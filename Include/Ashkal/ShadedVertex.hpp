#ifndef ASHKAL_SHADED_VERTEX_HPP
#define ASHKAL_SHADED_VERTEX_HPP
#include <array>
#include <cmath>
#include <tuple>
#include "Ashkal/Plane.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/ShadingTerm.hpp"
#include "Ashkal/TextureCoordinate.hpp"

namespace Ashkal {

  /** Stores a vertex after having a shader applied to it. */
  struct ShadedVertex {

    /** The position of the vertex. */
    Point m_position;

    /** The texture coordinate at the vertex. */
    TextureCoordinate m_uv;

    /** The shading applied to the vertex. */
    ShadingTerm m_shading;
  };

  /**
   * Computes the intersection point between the edge (a, b) and a plane,
   * interpolating all vertex attributes at the intersection.
   * @param a The first vertex of the edge.
   * @param b The second vertex of the edge.
   * @param plane The clipping plane.
   * @return A new ShadedVertex at the intersection point, with interpolated
   *         attributes.
   */
  inline ShadedVertex intersect(
      const ShadedVertex& a, const ShadedVertex& b, const Plane& plane) {
    auto distance_a = distance(plane, a.m_position);
    auto distance_b = distance(plane, b.m_position);
    auto t = distance_a / (distance_a - distance_b);
    auto result = a;
    result.m_position = a.m_position + t * (b.m_position - a.m_position);
    result.m_uv = TextureCoordinate(std::lerp(a.m_uv.m_u, b.m_uv.m_u, t),
      std::lerp(a.m_uv.m_v, b.m_uv.m_v, t));
    result.m_shading = ShadingTerm(
      lerp(a.m_shading.m_color, b.m_shading.m_color, t),
      std::lerp(a.m_shading.m_intensity, b.m_shading.m_intensity, t));
    return result;
  }

  /**
   * The maximum number of vertices that can result from clipping a triangle
   * against a plane.
   */
  const auto MAX_CLIP_COUNT = 4;

  using ClippedVertices = std::array<const ShadedVertex*, MAX_CLIP_COUNT>;

  /**
   * Clips a triangle against a plane, producing up to four vertices.
   * The function determines which vertices are in front of the plane and
   * computes intersection points as needed, returning pointers to the
   * resulting vertices.
   * @param v0 The first vertex of the triangle.
   * @param v1 The second vertex of the triangle.
   * @param v2 The third vertex of the triangle.
   * @param clipped_a Output parameter for the first clipped intersection vertex
   *        (if needed).
   * @param clipped_b Output parameter for the second clipped intersection
   *        vertex (if needed).
   * @param plane The clipping plane.
   * @return An array of pointers to the resulting vertices
   *         (up to 4, unused entries are nullptr).
   */
  inline ClippedVertices clip(const ShadedVertex& v0, const ShadedVertex& v1,
      const ShadedVertex& v2, ShadedVertex& clipped_a, ShadedVertex& clipped_b,
      const Plane& plane) {
    auto d0 = is_in_front(plane, v0.m_position);
    auto d1 = is_in_front(plane, v1.m_position);
    auto d2 = is_in_front(plane, v2.m_position);
    if(d0 && d1 && d2) {
      return {&v0, &v1, &v2, nullptr};
    } else if(!d0 && !d1 && !d2) {
      return {nullptr, nullptr, nullptr, nullptr};
    }
    auto count = 0;
    if(d0) {
      ++count;
    }
    if(d1) {
      ++count;
    }
    if(d2) {
      ++count;
    }
    if(count == 1) {
      auto [a, b, c] = [&] {
        if(d0) {
          return std::tuple(&v0, &v1, &v2);
        } else if(d1) {
          return std::tuple(&v1, &v2, &v0);
        }
        return std::tuple(&v2, &v0, &v1);
      }();
      clipped_a = intersect(*a, *b, plane);
      clipped_b = intersect(*a, *c, plane);
      return {a, &clipped_a, &clipped_b, nullptr};
    } else {
      auto [c, a, b] = [&] {
        if(!d0) {
          return std::tuple(&v0, &v1, &v2);
        } else if(!d1) {
          return std::tuple(&v1, &v2, &v0);
        }
        return std::tuple(&v2, &v0, &v1);
      }();
      clipped_a = intersect(*b, *c, plane);
      clipped_b = intersect(*a, *c, plane);
      return {a, b, &clipped_a, &clipped_b};
    }
  }
}

#endif
