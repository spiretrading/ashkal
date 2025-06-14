#include <doctest/doctest.h>
#include "Ashkal/BoundingBox.hpp"

using namespace Ashkal;

TEST_SUITE("BoundingBox") {
  TEST_CASE("default_constructor") {
    auto box = BoundingBox();
    CHECK(box.get_minimum() == Point(-0.5f, -0.5f, -0.5f));
    CHECK(box.get_maximum() == Point(0.5f, 0.5f, 0.5f));
  }

  TEST_CASE("apply") {
    auto box = BoundingBox();
    box.apply(translate(Vector(1, 2, 3)));
    auto expected_minimum = Point(0.5f, 1.5f, 2.5f);
    auto expected_maximum = Point(1.5f, 2.5f, 3.5f);
    CHECK(box.get_minimum().m_x == doctest::Approx(expected_minimum.m_x));
    CHECK(box.get_minimum().m_y == doctest::Approx(expected_minimum.m_y));
    CHECK(box.get_minimum().m_z == doctest::Approx(expected_minimum.m_z));
    CHECK(box.get_maximum().m_x == doctest::Approx(expected_maximum.m_x));
    CHECK(box.get_maximum().m_y == doctest::Approx(expected_maximum.m_y));
    CHECK(box.get_maximum().m_z == doctest::Approx(expected_maximum.m_z));
  }

  TEST_CASE("contains") {
    auto box = BoundingBox();
    auto point1 = Ashkal::Point(0, 0, 0);
    CHECK(contains(box, point1));
    auto point2 = Ashkal::Point(5, 0, 0);
    CHECK_FALSE(contains(box, point2));
  }

  TEST_CASE("intersects") {
    auto a = BoundingBox();
    auto b = BoundingBox();
    auto t = Matrix::IDENTITY();
    t.set(3, 0, 5);
    b.apply(t);
    CHECK_FALSE(intersects(a, b));
    auto t2 = Matrix::IDENTITY();
    t2.set(3, 0, 4.5f);
    a.apply(t2);
    CHECK(intersects(a, b));
  }
}
