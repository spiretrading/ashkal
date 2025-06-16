#include <numbers>
#include <doctest/doctest.h>
#include "Ashkal/BoundingBox.hpp"

using namespace Ashkal;

TEST_SUITE("BoundingBox") {
  TEST_CASE("default_constructor") {
    auto box = BoundingBox();
    CHECK(box.get_minimum() == Point(-0.5f, -0.5f, -0.5f));
    CHECK(box.get_maximum() == Point(0.5f, 0.5f, 0.5f));
  }

  TEST_CASE("custom_constructor") {
    auto box = BoundingBox(Point(-2, -3, -4), Point(2, 3, 4));
    CHECK(box.get_minimum() == Point(-2, -3, -4));
    CHECK(box.get_maximum() == Point(2, 3, 4));
  }

  TEST_CASE("apply_translation") {
    auto box = BoundingBox();
    box.apply(translate(Vector(1, 2, 3)));
    auto expected_min = Point(0.5f, 1.5f, 2.5f);
    auto expected_max = Point(1.5f, 2.5f, 3.5f);
    CHECK(box.get_minimum().m_x == doctest::Approx(expected_min.m_x));
    CHECK(box.get_minimum().m_y == doctest::Approx(expected_min.m_y));
    CHECK(box.get_minimum().m_z == doctest::Approx(expected_min.m_z));
    CHECK(box.get_maximum().m_x == doctest::Approx(expected_max.m_x));
    CHECK(box.get_maximum().m_y == doctest::Approx(expected_max.m_y));
    CHECK(box.get_maximum().m_z == doctest::Approx(expected_max.m_z));
  }

  TEST_CASE("apply_rotation") {
    auto box = BoundingBox(Point(1, 0, 0), Point(2, 1, 1));
    auto center = Vector((box.get_minimum().m_x + box.get_maximum().m_x) / 2,
      (box.get_minimum().m_y + box.get_maximum().m_y) / 2,
      (box.get_minimum().m_z + box.get_maximum().m_z) / 2);
    auto to_origin   = translate(-center);
    auto from_origin = translate(center);
    box.apply(from_origin * roll(std::numbers::pi / 2) * to_origin);
    CHECK(box.get_minimum() == Point(1, 0, 0));
    CHECK(box.get_maximum() == Point(2, 1, 1));
  }

  TEST_CASE("contains_inside_and_boundary") {
    auto box = BoundingBox(Point(-1, -1, -1), Point(1, 1, 1));
    CHECK(contains(box, Point(0, 0, 0)));
    CHECK(contains(box, Point(1, 0, 0)));
    CHECK(contains(box, Point(-1, 1, -1)));
    CHECK_FALSE(contains(box, Point(1.1f, 0, 0)));
  }

  TEST_CASE("intersects_overlap_and_touch") {
    auto a = BoundingBox(Point(-1, -1, -1), Point(1, 1, 1));
    auto b = BoundingBox(Point(1, 1, 1), Point(2, 2, 2));
    CHECK(intersects(a, b));
    auto c = BoundingBox(Point(1.1f, 1.1f, 1.1f), Point(2, 2, 2));
    CHECK_FALSE(intersects(a, c));
  }

  TEST_CASE("merge_overlapping") {
    auto a = BoundingBox(Point(-1, -1, -1), Point(1, 1, 1));
    auto b = BoundingBox(Point(0, 0, 0), Point(2, 2, 2));
    auto merged_box = merge(a, b);
    CHECK(merged_box.get_minimum() == Point(-1, -1, -1));
    CHECK(merged_box.get_maximum() == Point(2, 2, 2));
  }

  TEST_CASE("merge_disjoint") {
    auto a = BoundingBox(Point(-2, -2, -2), Point(-1, -1, -1));
    auto b = BoundingBox(Point(1, 1, 1), Point(2, 2, 2));
    auto merged_box = merge(a, b);
    CHECK(merged_box.get_minimum() == Point(-2, -2, -2));
    CHECK(merged_box.get_maximum() == Point(2, 2, 2));
  }

  TEST_CASE("merge_commutative") {
    auto a = BoundingBox(Point(-1, -1, -1), Point(0, 0, 0));
    auto b = BoundingBox(Point(1, 1, 1), Point(2, 2, 2));
    auto merged_box1 = merge(a, b);
    auto merged_box2 = merge(b, a);
    CHECK(merged_box1.get_minimum() == merged_box2.get_minimum());
    CHECK(merged_box1.get_maximum() == merged_box2.get_maximum());
  }
}
