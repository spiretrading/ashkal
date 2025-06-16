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

      /** Updates the frustum from the given Camera. */
      void update(const Camera& camera);

    private:
      std::array<Plane, PLANE_COUNT> m_planes;
  };

  inline const Plane& Frustum::get_plane(ClippingPlane plane) const {
    return m_planes[static_cast<std::size_t>(plane)];
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
