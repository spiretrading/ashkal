#include <doctest/doctest.h>
#include "Ashkal/Ray.hpp"

using namespace Ashkal;

TEST_SUITE("Ray") {
  TEST_CASE("compute_boundary") {
    auto ray = Ray(Point(0.5, 0.5, 1.1f), Vector(0, 0, -1));
    auto boundary = compute_boundary(ray, floor(ray.m_point), 1);
  }
}
