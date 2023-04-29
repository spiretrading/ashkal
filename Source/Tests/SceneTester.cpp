#include <doctest/doctest.h>
#include "Ashkal/Cube.hpp"
#include "Ashkal/Scene.hpp"

using namespace Ashkal;

namespace {
  static const auto V000 = Color(0, 0, 0, 0);
  static const auto V100 = Color(255, 0, 0, 0);
  static const auto V010 = Color(0, 255, 0, 0);
  static const auto V110 = Color(255, 255, 0, 0);
  static const auto V001 = Color(0, 0, 255, 0);
  static const auto V101 = Color(255, 0, 255, 0);
  static const auto V011 = Color(0, 255, 255, 0);
  static const auto V111 = Color(255, 255, 255, 0);

  class TestModel : public Model {
    public:
      Point end() const override {
        return Point(2, 2, 2);
      }

      Voxel get(Point point) const override {
        if(point.m_x == 0 && point.m_y == 0 && point.m_z == 0) {
          return Voxel(V000);
        } else if(point.m_x == 1 && point.m_y == 0 && point.m_z == 0) {
          return Voxel(V100);
        } else if(point.m_x == 0 && point.m_y == 1 && point.m_z == 0) {
          return Voxel(V010);
        } else if(point.m_x == 1 && point.m_y == 1 && point.m_z == 0) {
          return Voxel(V110);
        } else if(point.m_x == 0 && point.m_y == 0 && point.m_z == 1) {
          return Voxel(V001);
        } else if(point.m_x == 1 && point.m_y == 0 && point.m_z == 1) {
          return Voxel(V101);
        } else if(point.m_x == 0 && point.m_y == 1 && point.m_z == 1) {
          return Voxel(V011);
        } else if(point.m_x == 1 && point.m_y == 1 && point.m_z == 1) {
          return Voxel(V111);
        }
        return Voxel::NONE();
      }
  };
}

TEST_SUITE("Scene") {
  TEST_CASE("intersect") {
    auto scene = Scene();
    scene.add(std::make_shared<TestModel>());
    auto intersection000 = scene.intersect(Point(0, 0, -10), Vector(0, 0, 1));
    CHECK(intersection000.m_voxel.m_color == V000);
    CHECK(intersection000.m_position == Point(0, 0, 0));
    auto intersection100 = scene.intersect(Point(1, 0, -10), Vector(0, 0, 1));
    CHECK(intersection100.m_voxel.m_color == V100);
    CHECK(intersection100.m_position == Point(1, 0, 0));
    auto intersection010 = scene.intersect(Point(0, 1, -10), Vector(0, 0, 1));
    CHECK(intersection010.m_voxel.m_color == V010);
    CHECK(intersection010.m_position == Point(0, 1, 0));
    auto intersection110 = scene.intersect(Point(1, 1, -10), Vector(0, 0, 1));
    CHECK(intersection110.m_voxel.m_color == V110);
    CHECK(intersection110.m_position == Point(1, 1, 0));
    auto intersection001 = scene.intersect(Point(0, 0, 10), Vector(0, 0, -1));
    CHECK(intersection001.m_voxel.m_color == V001);
    CHECK(intersection001.m_position == Point(0, 0, 1));
    auto intersection101 = scene.intersect(Point(1, 0, 10), Vector(0, 0, -1));
    CHECK(intersection101.m_voxel.m_color == V101);
    CHECK(intersection101.m_position == Point(1, 0, 1));
    auto intersection011 = scene.intersect(Point(0, 1, 10), Vector(0, 0, -1));
    CHECK(intersection011.m_voxel.m_color == V011);
    CHECK(intersection011.m_position == Point(0, 1, 1));
    auto intersection111 = scene.intersect(Point(1, 1, 10), Vector(0, 0, -1));
    CHECK(intersection111.m_voxel.m_color == V111);
    CHECK(intersection111.m_position == Point(1, 1, 1));
  }
}
