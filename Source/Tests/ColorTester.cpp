#include <algorithm>
#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/Color.hpp"

using namespace Ashkal;

TEST_SUITE("Color") {
  TEST_CASE("default_constructor") {
    auto c = Color();
    CHECK(c.get_red() == 0);
    CHECK(c.get_green() == 0);
    CHECK(c.get_blue() == 0);
    CHECK(c.get_alpha() == 255);
  }

  TEST_CASE("stream") {
    auto oss = std::ostringstream();
    oss << Color(1, 2, 3, 4);
    CHECK(oss.str() == "Color(1, 2, 3, 4)");
  }

  TEST_CASE("as_rgba") {
    auto c = Color(0xAA, 0xBB, 0xCC, 0xDD);
    CHECK(c.as_rgba() == 0xAABBCCDDu);
  }

  TEST_CASE("channel_setters") {
    auto c = Color();
    c.set_red(10);
    c.set_green(20);
    c.set_blue(30);
    c.set_alpha(40);
    CHECK(c.get_red()   == 10);
    CHECK(c.get_green() == 20);
    CHECK(c.get_blue()  == 30);
    CHECK(c.get_alpha() == 40);
  }

  TEST_CASE("addition") {
    auto a = Color(100, 150, 200, 123);
    auto b = Color(100, 150, 100, 45);
    auto sum = a + b;
    CHECK(sum.get_red() == 200);
    CHECK(sum.get_green() == 255);
    CHECK(sum.get_blue() == 255);
    CHECK(sum.get_alpha() == 123);
    auto c1 = Color(200, 200, 200, 50);
    auto c2 = Color(100, 100, 100, 60);
    auto sat = c1 + c2;
    CHECK(sat.get_red() == 255);
    CHECK(sat.get_green() == 255);
    CHECK(sat.get_blue() == 255);
    CHECK(sat.get_alpha() == 50);
  }

  TEST_CASE("lerp_left") {
    auto left = Color(10, 20, 30, 40);
    auto right = Color(200, 210, 220, 230);
    auto result = lerp(left, right, 0);
    CHECK(result == left);
  }

  TEST_CASE("lerp_right") {
    auto left = Color(10, 20, 30, 40);
    auto right = Color(200, 210, 220, 230);
    auto result = lerp(left, right, 1);
    CHECK(result == right);
  }

  TEST_CASE("lerp_middle") {
    auto left = Color(0, 0, 0, 0);
    auto right = Color(255, 255, 255, 255);
    auto result = lerp(left, right, 0.5f);
    CHECK(result == Color(127, 127, 127, 127));
  }

  TEST_CASE("lerp") {
    auto left = Color(100, 150, 200, 50);
    auto right = Color(200, 100, 50, 150);
    auto t = 0.25f;
    auto result = lerp(left, right, t);
    auto expected_color = Color(
      static_cast<uint8_t>(std::lerp(100.0f, 200.0f, t)),
      static_cast<uint8_t>(std::lerp(150.0f, 100.0f, t)),
      static_cast<uint8_t>(std::lerp(200.0f,  50.0f, t)),
      static_cast<uint8_t>(std::lerp( 50.0f, 150.0f, t)));
    CHECK(result == expected_color);
  }
}
