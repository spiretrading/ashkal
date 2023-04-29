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
    auto n1 = compute_surface_normal(Point(1, 0, 0), Point(1.f, 0.f, 0.f));
    auto n2 = compute_surface_normal(Point(0, 0, 0), Point(1.f, 0.f, 0.f));
  }
}
