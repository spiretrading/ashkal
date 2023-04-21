#include <doctest/doctest.h>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include "Ashkal/Camera.hpp"

using namespace Ashkal;

TEST_SUITE("Camera") {
  TEST_CASE("tilt") {
/*
    auto camera = Camera(Point(1, 0, 1), Vector(-1, 0, 0), Vector(0, 1, 0));
    auto p = camera.get_position();
    auto d = camera.get_direction();
    auto o = camera.get_orientation();
    std::cout << p << " " << d << " " << o << std::endl;
    tilt(camera, 0, std::numbers::pi_v<float> / 2);
    p = camera.get_position();
    d = camera.get_direction();
    o = camera.get_orientation();
    std::cout << p << " " << d << " " << o;
    CHECK(camera.get_position() == Point(1, 0, 1));
    CHECK(camera.get_direction() == Vector(0, 1, 0));
    CHECK(camera.get_orientation() == Vector(1, 0, 0));
*/
  }
}
