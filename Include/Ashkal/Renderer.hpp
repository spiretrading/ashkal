#ifndef ASHKAL_RENDERER_HPP
#define ASHKAL_RENDERER_HPP
#include <cmath>
#include "Ashkal/Camera.hpp"
#include "Ashkal/Point.hpp"

namespace Ashkal {

  /** Stores screen pixel coordinates. */
  struct ScreenCoordinate {

    /** The x coordinate in pixels. */
    int m_x;

    /** The y coordinate in pixels. */
    int m_y;
  };

  /** Stores screen pixel coordinates as floats. */
  struct FloatScreenCoordinate {

    /** The x coordinate in pixels. */
    float m_x;

    /** The y coordinate in pixels. */
    float m_y;
  };

  /**
   * Projects a 3D point in camera space onto a 2D screen.
   * @param point  The 3D point in camera space to project.
   * @param camera The camera in whose space is being projected onto a 2D
   *        screen.
   * @param width The width of the viewport in pixels.
   * @param height The height of the viewport in pixels.
   * @return A ScreenCoordinate containing the projected pixel coordinates.
   */
  inline ScreenCoordinate project_to_screen(
      const Point& point, const Camera& camera, int width, int height) {
    const auto THRESHOLD = 1e-5f;
    auto near_z = -point.m_z;
    if(near_z <= THRESHOLD) {
      near_z = THRESHOLD;
    }
    auto clip_x = camera.get_horizontal_focal_length() * point.m_x;
    auto clip_y = camera.get_focal_length() * point.m_y;
    auto perspective_divide = 1 / near_z;
    auto normalized_x = clip_x * perspective_divide;
    auto normalized_y = clip_y * perspective_divide;
    auto fx = (normalized_x + 1) * 0.5f * (width - 1);
    auto fy = (1 - (normalized_y + 1) * 0.5f) * (height - 1);
    return ScreenCoordinate(int(fx), int(fy));
  }
}

#endif
