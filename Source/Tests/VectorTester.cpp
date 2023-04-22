#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/Vector.hpp"

using namespace Ashkal;

TEST_SUITE("Vector") {
  TEST_CASE("default_constructor") {
    auto v = Vector();
    CHECK(v.m_x == 0.0f);
    CHECK(v.m_y == 0.0f);
    CHECK(v.m_z == 0.0f);
  }

  TEST_CASE("component_constructor") {
    auto v = Vector(1.0f, 2.0f, 3.0f);
    CHECK(v.m_x == 1.0f);
    CHECK(v.m_y == 2.0f);
    CHECK(v.m_z == 3.0f);
  }

  TEST_CASE("point_constructor") {
    auto p = Point(1.0f, 2.0f, 3.0f);
    auto v = Vector(p);
    CHECK(v.m_x == p.m_x);
    CHECK(v.m_y == p.m_y);
    CHECK(v.m_z == p.m_z);
  }

  TEST_CASE("equality_operators") {
    auto v1 = Vector(1.0f, 2.0f, 3.0f);
    auto v2 = Vector(1.0f, 2.0f, 3.0f);
    auto v3 = Vector(1.5f, 2.5f, 3.5f);
    CHECK(v1 == v2);
    CHECK(v1 != v3);
  }

  TEST_CASE("stream_insertion") {
    auto oss = std::ostringstream();
    oss << Vector(1.0f, 2.0f, 3.0f);
    CHECK(oss.str() == "Vector(1, 2, 3)");
  }

  TEST_CASE("negation") {
    auto v = Vector(1.0f, 2.0f, 3.0f);
    auto negation = Vector(-1.0f, -2.0f, -3.0f);
    CHECK(-v == negation);
  }

  TEST_CASE("point_subtraction") {
    auto p1 = Point(1.0f, 2.0f, 3.0f);
    auto p2 = Point(4.0f, 5.0f, 6.0f);
    auto result = Vector(-3.0f, -3.0f, -3.0f);
    CHECK(p1 - p2 == result);
  }

  TEST_CASE("vector_subtraction") {
    auto v1 = Vector(1.0f, 2.0f, 3.0f);
    auto v2 = Vector(4.0f, 5.0f, 6.0f);
    auto result = Vector(-3.0f, -3.0f, -3.0f);
    CHECK(v1 - v2 == result);
  }

  TEST_CASE("addition") {
    auto v1 = Vector(1.0f, 2.0f, 3.0f);
    auto v2 = Vector(4.0f, 5.0f, 6.0f);
    auto result = Vector(5.0f, 7.0f, 9.0f);
    CHECK(v1 + v2 == result);
  }

  TEST_CASE("int_multiplication") {
    auto v = Vector(1.0f, 2.0f, 3.0f);
    auto result = Vector(2.0f, 4.0f, 6.0f);
    CHECK(2 * v == result);
  }

  TEST_CASE("float_multiplication") {
    auto v = Vector(1.0f, 2.0f, 3.0f);
    auto result = Vector(3.0f, 6.0f, 9.0f);
    CHECK(3.0f * v == result);
  }

  TEST_CASE("int_division") {
    auto v = Vector(2.0f, 4.0f, 6.0f);
    auto result = Vector(1.0f, 2.0f, 3.0f);
    CHECK(v / 2 == result);
  }

  TEST_CASE("float_division") {
    auto v = Vector(3.0f, 6.0f, 9.0f);
    auto result = Vector(1.0f, 2.0f, 3.0f);
    CHECK(v / 3.0f == result);
  }

  TEST_CASE("point_addition") {
    auto p = Point(1.0f, 2.0f, 3.0f);
    auto v = Vector(4.0f, 5.0f, 6.0f);
    auto result = Point(5.0f, 7.0f, 9.0f);
    CHECK(p + v == result);
  }

  TEST_CASE("point_subtraction") {
    auto p = Point(4.0f, 5.0f, 6.0f);
    auto v = Vector(1.0f, 2.0f, 3.0f);
    auto result = Point(3.0f, 3.0f, 3.0f);
    CHECK(p - v == result);
  }

  TEST_CASE("cross") {
    auto v1 = Vector(1.0f, 0.0f, 0.0f);
    auto v2 = Vector(0.0f, 1.0f, 0.0f);
    auto result = Vector(0.0f, 0.0f, 1.0f);
    CHECK(cross(v1, v2) == result);
  }

  TEST_CASE("dot") {
    auto v1 = Vector(1.0f, 2.0f, 3.0f);
    auto v2 = Vector(4.0f, 5.0f, 6.0f);
    auto result = 32.0f;
    CHECK(dot(v1, v2) == result);
  }

  TEST_CASE("magnitude") {
    auto v = Vector(1.0f, 2.0f, 2.0f);
    auto result = 3.0f;
    CHECK(magnitude(v) == result);
  }

  TEST_CASE("normalize") {
    auto v = Vector(1.0f, 2.0f, 2.0f);
    auto result = Vector(1.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f);
    auto normalized = normalize(v);
    CHECK(normalized.m_x == doctest::Approx(result.m_x));
    CHECK(normalized.m_y == doctest::Approx(result.m_y));
    CHECK(normalized.m_z == doctest::Approx(result.m_z));
  }
}
