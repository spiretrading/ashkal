#include <doctest/doctest.h>
#include "Ashkal/Plane.hpp"
#include "Ashkal/Vector.hpp"

using namespace Ashkal;

TEST_SUITE("Plane") {
  TEST_CASE("distance_point_on_plane") {
    auto plane = Plane(Vector(1, 0, 0), 0);
    auto on_plane = Point(0, 5, -3);
    CHECK(distance(plane, on_plane) == doctest::Approx(0.0f));
  }

  TEST_CASE("distance_point_in_front_and_behind") {
    auto plane = Plane(Vector(1, 0, 0), 0);
    auto in_front = Point(2.5f, 0, 0);
    auto behind = Point(-1.5f, 0, 0);
    CHECK(distance(plane, in_front)  == doctest::Approx(2.5f));
    CHECK(distance(plane, behind)   == doctest::Approx(-1.5f));
  }

  TEST_CASE("distance_plane_with_offset") {
    auto plane = Plane(Vector(0, 1, 0), -5);
    auto above_plane = Point(5, 6, 0);
    auto below_plane = Point(3, 4, 0);
    CHECK(distance(plane, above_plane) == doctest::Approx(1));
    CHECK(distance(plane, below_plane) == doctest::Approx(-1));
  }

  TEST_CASE("make_plane_xy") {
    auto a = Point(0, 0, 0);
    auto b = Point(1, 0, 0);
    auto c = Point(0, 1, 0);
    auto plane = make_plane(a, b, c);
    CHECK(plane.m_normal.m_x == doctest::Approx(0.0f));
    CHECK(plane.m_normal.m_y == doctest::Approx(0.0f));
    CHECK(plane.m_normal.m_z == doctest::Approx(-1.0f));
    CHECK(plane.m_d == doctest::Approx(0.0f));
  }

  TEST_CASE("make_plane_XZ") {
    auto a = Point(0, 0, 0);
    auto b = Point(1, 0, 0);
    auto c = Point(0, 0, 1);
    auto plane = make_plane(a, b, c);
    CHECK(plane.m_normal.m_x == doctest::Approx(0.0f));
    CHECK(plane.m_normal.m_y == doctest::Approx(1.0f));
    CHECK(plane.m_normal.m_z == doctest::Approx(0.0f));
    CHECK(plane.m_d == doctest::Approx(0.0f));
  }

  TEST_CASE("make_plane_xy_offset") {
    auto a = Point(0, 0, 1);
    auto b = Point(1, 0, 1);
    auto c = Point(0, 1, 1);
    auto plane = make_plane(a, b, c);
    CHECK(plane.m_normal.m_x == doctest::Approx(0.0f));
    CHECK(plane.m_normal.m_y == doctest::Approx(0.0f));
    CHECK(plane.m_normal.m_z == doctest::Approx(-1.0f));
    CHECK(plane.m_d == doctest::Approx(1.0f));
  }

  TEST_CASE("make_plane_generic") {
    auto a = Point(1, 0, 0);
    auto b = Point(0, 1, 0);
    auto c = Point(0, 0, 1);
    auto plane = make_plane(a, b, c);
    auto d = 1 / std::sqrt(3);
    CHECK(plane.m_normal.m_x == doctest::Approx(-d));
    CHECK(plane.m_normal.m_y == doctest::Approx(-d));
    CHECK(plane.m_normal.m_z == doctest::Approx(-d));
    CHECK(plane.m_d == doctest::Approx(d));
  }
}
