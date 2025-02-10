#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/Voxel.hpp"

using namespace Ashkal;

TEST_SUITE("Voxel") {
  TEST_CASE("default_constructor") {
    auto v = Voxel();
    CHECK(v.m_color == Color());
  }

  TEST_CASE("stream") {
    auto oss = std::ostringstream();
    oss << Voxel(Color(255, 0, 255, 0));
    CHECK(oss.str() == "Voxel(Color(255, 0, 255, 0))");
  }

  TEST_CASE("compute_surface_normal") {
    auto intersection = Point(1, 0.5f, .1f);
    auto n1 = compute_surface_normal(intersection, floor(intersection));
    CHECK(n1 == Vector(1, 0, 0));
  }
}
