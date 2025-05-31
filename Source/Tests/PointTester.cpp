#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/Point.hpp"

using namespace Ashkal;

TEST_SUITE("Point") {
  TEST_CASE("default_constructor") {
    auto p = Point();
    CHECK(p.m_x == 0.0f);
    CHECK(p.m_y == 0.0f);
    CHECK(p.m_z == 0.0f);
  }

  TEST_CASE("three_way_comparison") {
    auto p1 = Point(1.0f, 2.0f, 3.0f);
    auto p2 = Point(1.0f, 2.0f, 3.0f);
    auto p3 = Point(1.5f, 2.5f, 3.5f);
    CHECK(p1 == p2);
    CHECK(p1 < p3);
    CHECK(p3 > p1);
  }

  TEST_CASE("floor") {
    auto p1 = Point(1.5f, 2.5f, 3.5f);
    auto p2 = Point(1.0f, 2.0f, 3.0f);
    auto result = floor(p1);
    CHECK(result.m_x == p2.m_x);
    CHECK(result.m_y == p2.m_y);
    CHECK(result.m_z == p2.m_z);
  }

  TEST_CASE("contains") {
    auto start = Point(1.0f, 2.0f, 3.0f);
    auto end = Point(4.0f, 5.0f, 6.0f);
    auto inside = Point(3.0f, 4.0f, 5.0f);
    auto outside = Point(0.0f, 1.0f, 2.0f);
    CHECK(contains(start, end, inside));
    CHECK(!contains(start, end, outside));
  }

  TEST_CASE("stream") {
    auto oss = std::ostringstream();
    oss << Point(1.0f, 2.0f, 3.0f);
    CHECK(oss.str() == "Point(1, 2, 3)");
  }
}
