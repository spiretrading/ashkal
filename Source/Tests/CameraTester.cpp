#include <doctest/doctest.h>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include "Ashkal/Camera.hpp"

using namespace Ashkal;

TEST_SUITE("Camera") {
  TEST_CASE("default_constructor") {
    auto camera = Camera();
    CHECK(camera.get_position() == Point(0, 0, 0));
    CHECK(camera.get_direction() == Vector(0, 0, 1));
    CHECK(camera.get_orientation() == Vector(0, 1, 0));
    CHECK(camera.get_right() == Vector(1, 0, 0));
  }

  TEST_CASE("component_constructor") {
    auto position = Point(1, 2, 3);
    auto direction = Vector(0, 0, 1);
    auto orientation = Vector(0, 1, 0);
    auto camera = Camera(position, direction, orientation);
    CHECK(camera.get_position() == position);
    CHECK(camera.get_direction() == direction);
    CHECK(camera.get_orientation() == orientation);
    CHECK(camera.get_right() == Vector(1, 0, 0));
  }

  TEST_CASE("apply") {
    auto camera = Camera();
    auto transform = translate(Vector(1, 1, 1));
    camera.apply(transform);
    CHECK(camera.get_position() == Point(1, 1, 1));
    CHECK(camera.get_direction() == Vector(0, 0, 1));
    CHECK(camera.get_orientation() == Vector(0, 1, 0));
    CHECK(camera.get_right() == Vector(1, 0, 0));
  }

  TEST_CASE("movement") {
    auto camera = Camera();
    auto distance = 5.f;
    SUBCASE("move_forward") {
      move_forward(camera, distance);
      CHECK(camera.get_position() == Point(0, 0, 5));
    }
    SUBCASE("move_backward") {
      move_backward(camera, distance);
      CHECK(camera.get_position() == Point(0, 0, -5));
    }
    SUBCASE("move_left") {
      move_left(camera, distance);
      CHECK(camera.get_position() == Point(-5, 0, 0));
    }
    SUBCASE("move_right") {
      move_right(camera, distance);
      CHECK(camera.get_position() == Point(5, 0, 0));
    }
  }

  TEST_CASE("tilt") {
    auto camera = Camera();
    SUBCASE("45-degree tilt along X-axis") {
      auto tilt_x = std::numbers::pi_v<float> / 4.f;
      tilt(camera, tilt_x, 0.f);
      CHECK(camera.get_position() == Point(0, 0, 0));
      CHECK(camera.get_direction().m_x ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_direction().m_y ==
        doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_direction().m_z ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_x ==
        doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_y ==
        doctest::Approx(1.f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_z ==
        doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_right().m_x ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_right().m_y == doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_right().m_z ==
        doctest::Approx(-0.7071f).epsilon(0.0001f));
    }
    SUBCASE("45-degree tilt along Y-axis") {
      auto tilt_y = std::numbers::pi_v<float> / 4.f;
      tilt(camera, 0.f, tilt_y);
      CHECK(camera.get_position() == Point(0, 0, 0));
      CHECK(camera.get_direction().m_x ==
        doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_direction().m_y ==
        doctest::Approx(-0.7071f).epsilon(0.0001f));
      CHECK(camera.get_direction().m_z ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_x ==
        doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_y ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_orientation().m_z ==
        doctest::Approx(0.7071f).epsilon(0.0001f));
      CHECK(camera.get_right().m_x == doctest::Approx(1.f).epsilon(0.0001f));
      CHECK(camera.get_right().m_y == doctest::Approx(0.f).epsilon(0.0001f));
      CHECK(camera.get_right().m_z == doctest::Approx(0.f).epsilon(0.0001f));
    }
  }

  TEST_CASE("rotation") {
    auto camera = Camera();
    auto rotation_matrix = yaw(std::numbers::pi_v<float> / 2.f) *
      pitch(std::numbers::pi_v<float> / 4.f);
    camera.apply(rotation_matrix);
    CHECK(camera.get_position() == Point(0, 0, 0));
    CHECK(camera.get_direction().m_x == doctest::Approx(1.f).epsilon(0.0001f));
    CHECK(camera.get_direction().m_y == doctest::Approx(0.f).epsilon(0.0001f));
    CHECK(camera.get_direction().m_z == doctest::Approx(0.f).epsilon(0.0001f));
    CHECK(camera.get_orientation().m_x ==
      doctest::Approx(0.f).epsilon(0.0001f));
    CHECK(camera.get_orientation().m_y ==
      doctest::Approx(0.7071f).epsilon(0.0001f));
    CHECK(camera.get_orientation().m_z ==
      doctest::Approx(-0.7071f).epsilon(0.0001f));
    CHECK(camera.get_right().m_x == doctest::Approx(0.f).epsilon(0.0001f));
    CHECK(camera.get_right().m_y == doctest::Approx(-0.7071f).epsilon(0.0001f));
    CHECK(camera.get_right().m_z == doctest::Approx(-0.7071f).epsilon(0.0001f));
  }
}
