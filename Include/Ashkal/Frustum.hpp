#ifndef ASHKAL_FRUSTUM_HPP
#define ASHKAL_FRUSTUM_HPP
#include <array>
#include "Ashkal/BoundingBox.hpp"
#include "Ashkal/Plane.hpp"
#ifdef NEAR
  #undef NEAR
#endif
#ifdef FAR
  #undef FAR
#endif

namespace Ashkal {
  class Camera;

  /** Represents a camera view frustum, defined by six clipping planes. */
  class Frustum {
    public:

      /** Enumerates the six clipping planes of a view frustum. */
      enum class ClippingPlane {

        /** The left facing plane. */
        LEFT,

        /** The right facing plane. */
        RIGHT,

        /** The bottom facing plane. */
        BOTTOM,

        /** The top facing plane. */
        TOP,

        /** The near plane facing the camera. */
        NEAR,

        /** The far plane facing away from the camera. */
        FAR
      };

      /** The number of clipping planes enumerated. */
      static const auto PLANE_COUNT = 6;

      /** Returns the chosen clipping plane. */
      const Plane& get_plane(ClippingPlane plane) const;

      /**
       * Updates the frustum.
       * @param position The position of the origin.
       * @param direction The normalized direction of the interior of the frustum.
       * @param orientation The normalized direction pointing up.
       * @param near_plane The distance from the origin to the near plane.
       * @param far_plane The distance to the far plane.
       * @param aspect_ratio The aspect ratio.
       * @param field_of_view The vertical field of view in radians.
       */
      void update(const Point& position, const Vector& direction,
        const Vector& orientation, float near_plane, float far_plane,
        float aspect_ratio, float field_of_view);

      /** Updates the frustum from the given Camera. */
      void update(const Camera& camera);

    private:
      std::array<Plane, PLANE_COUNT> m_planes;
  };

  inline const Plane& Frustum::get_plane(ClippingPlane plane) const {
    return m_planes[static_cast<std::size_t>(plane)];
  }

  inline void Frustum::update(const Point& position, const Vector& direction,
      const Vector& orientation, float near_plane, float far_plane,
      float aspect_ratio, float field_of_view) {
    auto tan_half_field_of_view = std::tan(0.5f * field_of_view);
    auto right = cross(orientation, direction);
    auto near_plane_center = position + -near_plane * direction;
    auto near_plane_half_height = tan_half_field_of_view * -near_plane;
    auto near_plane_half_width = aspect_ratio * near_plane_half_height;
    auto near_plane_top_left = near_plane_center +
      near_plane_half_height * orientation - near_plane_half_width * right;
    auto near_plane_top_right =
      near_plane_top_left + 2 * near_plane_half_width * right;
    auto near_plane_bottom_left = near_plane_top_left -
      2 * near_plane_half_height * orientation;
    auto near_plane_bottom_right =
      near_plane_bottom_left + 2 * near_plane_half_width * right;
    auto far_plane_center = position + -far_plane * direction;
    auto far_plane_half_height = tan_half_field_of_view * -far_plane;
    auto far_plane_half_width = aspect_ratio * far_plane_half_height;
    auto far_plane_top_left = far_plane_center +
      far_plane_half_height * orientation - far_plane_half_width * right;
    auto far_plane_top_right =
      far_plane_top_left + 2 * far_plane_half_width * right;
    auto far_plane_bottom_left =
      far_plane_top_left - 2 * far_plane_half_height * orientation;
    auto far_plane_bottom_right =
      far_plane_bottom_left + 2 * far_plane_half_width * right;
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::LEFT)] =
      make_plane(position, near_plane_bottom_left, near_plane_top_left);
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::RIGHT)] =
      make_plane(position, near_plane_top_right, near_plane_bottom_right);
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::BOTTOM)] =
      make_plane(position, near_plane_bottom_right, near_plane_bottom_left);
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::TOP)] =
      make_plane(position, near_plane_top_left, near_plane_top_right);
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::NEAR)] =
      make_plane(
        near_plane_top_left, near_plane_top_right, near_plane_bottom_right);
    m_planes[static_cast<std::size_t>(Frustum::ClippingPlane::FAR)] =
      make_plane(
        far_plane_top_right, far_plane_top_left, far_plane_bottom_left);
  }

  /**
   * Tests whether a bounding box intersects or is inside the frustum.
   * @param frustum The view frustum.
   * @param box The bounding box to test.
   * @return True iff any part of the box lies within the frustum.
   */
  inline bool intersects(const Frustum& frustum, const BoundingBox& box) {
    for(auto i = std::size_t(0); i < Frustum::PLANE_COUNT; ++i) {
      auto& plane = frustum.get_plane(static_cast<Frustum::ClippingPlane>(i));
      auto corner = Point(
        plane.m_normal.m_x >= 0 ? box.get_maximum().m_x : box.get_minimum().m_x,
        plane.m_normal.m_y >= 0 ? box.get_maximum().m_y : box.get_minimum().m_y,
        plane.m_normal.m_z >= 0 ?
          box.get_maximum().m_z : box.get_minimum().m_z);
      if(distance(plane, corner) < 0) {
        return false;
      }
    }
    return true;
  }
}

#endif
