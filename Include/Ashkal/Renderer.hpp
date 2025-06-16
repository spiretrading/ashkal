#ifndef ASHKAL_RENDERER_HPP
#define ASHKAL_RENDERER_HPP
#include <cmath>
#include "Ashkal/Camera.hpp"
#include "Ashkal/Point.hpp"

namespace Ashkal {
  struct ScreenCoordinate {
    int m_x;
    int m_y;
  };

  inline ScreenCoordinate project_to_screen(
      const Point& point, const Camera& camera, int width, int height) {
/*
    const auto THRESHOLD = 1e-5f;
    auto near_z = -point.m_z;
    if(near_z <= THRESHOLD) {
      near_z = THRESHOLD;
    }
    auto f = 1 / std::tan(0.5f * camera.get_field_of_view());
    auto clip_x = f * point.m_x / camera.get_aspect_ratio();
    auto clip_y = f * point.m_y;
    auto inverse_z = 1 / near_z;
    auto normalized_x = clip_x * inverse_z;
    auto normalized_y = clip_y * inverse_z;
    auto fx = (normalized_x + 1) * 0.5f * (width - 1);
    auto fy = (1 - (normalized_y + 1) * 0.5f) * (height - 1);
    return ScreenCoordinate(int(fx), int(fy));
*/
  const auto THRESHOLD = 1e-5f;
  auto zz = -point.m_z;
  if(zz <= THRESHOLD) {
    zz = THRESHOLD;
  }
  auto f = 1 / std::tan(0.5f * camera.get_field_of_view());
  auto clip_x = f * point.m_x / camera.get_aspect_ratio();
  auto clip_y = f * point.m_y;
  auto perspective = 1 / zz;
  auto normalized_x = clip_x * perspective;
  auto normalized_y = clip_y * perspective;
  auto fx = (normalized_x + 1) * 0.5f * (width - 1);
  auto fy = (1 - (normalized_y + 1) * 0.5f) * (height - 1);
  return ScreenCoordinate(int(fx), int(fy));
  }
}

#endif
