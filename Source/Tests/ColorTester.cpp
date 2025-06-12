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
}
