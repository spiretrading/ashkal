#include <doctest/doctest.h>
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/DirectionalLight.hpp"

using namespace Ashkal;

TEST_SUITE("DirectionalLight") {
  TEST_CASE("apply_directional_light") {
    auto light =
      DirectionalLight(Vector(0, 0, 1), Color(255, 255, 255, 0), 1.f);
    auto color = Color(255, 0, 0, 0);
  }
}
