#include <sstream>
#include <doctest/doctest.h>
#include "Ashkal/AmbientLight.hpp"

using namespace Ashkal;

TEST_SUITE("AmbientLight") {
  TEST_CASE("constructor") {
    auto light = AmbientLight(Color(10, 20, 30), 0.5f);
    CHECK(light.m_color == Color(10, 20, 30));
    CHECK(light.m_intensity == doctest::Approx(0.5f));
  }

  TEST_CASE("calculate_shading") {
    auto light = AmbientLight(Color(100, 150, 200), 0.75f);
    auto shading = calculate_shading(light);
    CHECK(shading.m_color == Color(100, 150, 200));
    CHECK(shading.m_intensity == doctest::Approx(0.75f));
  }

  TEST_CASE("stream") {
    auto light = AmbientLight(Color(1, 2, 3), 0.25f);
    auto ss = std::ostringstream();
    ss << light;
    auto str = ss.str();
    CHECK(str.find("AmbientLight(") != std::string::npos);
    CHECK(str.find("0.25") != std::string::npos);
  }
}