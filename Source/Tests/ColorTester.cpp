#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/Color.hpp"

using namespace Ashkal;

TEST_SUITE("Color") {
  TEST_CASE("default_constructor") {
    auto c = Color();
    CHECK(c.m_red == 0);
    CHECK(c.m_green == 0);
    CHECK(c.m_blue == 0);
    CHECK(c.m_alpha == 0);
  }

  TEST_CASE("stream") {
    auto oss = std::ostringstream();
    oss << Color(1, 2, 3, 4);
    CHECK(oss.str() == "Color(1, 2, 3, 4)");
  }
}
