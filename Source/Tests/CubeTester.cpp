#include <doctest/doctest.h>
#include "Ashkal/Cube.hpp"

using namespace Ashkal;

TEST_SUITE("Cube") {
  TEST_CASE("constructor") {
    auto size = 5;
    auto color = Color(255, 0, 0, 255);
    auto cube = Cube(size, color);
    SUBCASE("end") {
      auto expected_end = Point(static_cast<float>(size),
        static_cast<float>(size), static_cast<float>(size));
      CHECK(cube.end() == expected_end);
    }
    SUBCASE("interior") {
      for(auto x = 0; x < size; ++x) {
        for(auto y = 0; y < size; ++y) {
          for(auto z = 0; z < size; ++z) {
            auto point = Point(static_cast<float>(x), static_cast<float>(y),
              static_cast<float>(z));
            auto voxel = cube.get(point);
            CHECK(voxel.m_color == color);
          }
        }
      }
    }
    SUBCASE("exterior") {
      auto margin = 2;
      for(auto x = -margin; x < size + margin; ++x) {
        for(auto y = -margin; y < size + margin; ++y) {
          for(auto z = -margin; z < size + margin; ++z) {
            if(x < 0 || x >= size || y < 0 || y >= size || z < 0 || z >= size) {
              auto point = Point(static_cast<float>(x), static_cast<float>(y),
                static_cast<float>(z));
              auto voxel = cube.get(point);
              CHECK(voxel == Ashkal::Voxel::NONE());
            }
          }
        }
      }
    }
  }
}
