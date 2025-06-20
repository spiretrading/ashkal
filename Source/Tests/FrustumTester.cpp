#include <doctest/doctest.h>
#include "Ashkal/Camera.hpp"
#include "Ashkal/Frustum.hpp"
#include "Ashkal/Renderer.hpp"

using namespace Ashkal;

TEST_SUITE("Frustum") {
  TEST_CASE("frustum_from_camera") {
    auto camera = Camera(1);
    auto& frustum = camera.get_frustum();
    auto& left_plane = frustum.get_plane(Frustum::ClippingPlane::LEFT);
    auto& right_plane = frustum.get_plane(Frustum::ClippingPlane::RIGHT);
    auto& top_plane = frustum.get_plane(Frustum::ClippingPlane::TOP);
    auto& bottom_plane = frustum.get_plane(Frustum::ClippingPlane::BOTTOM);
    auto& near_plane = frustum.get_plane(Frustum::ClippingPlane::NEAR);
    auto& far_plane = frustum.get_plane(Frustum::ClippingPlane::FAR);
    CHECK(&left_plane != &right_plane);
    CHECK(&left_plane != &top_plane);
    CHECK(&left_plane != &bottom_plane);
    CHECK(&left_plane != &near_plane);
    CHECK(&left_plane != &far_plane);
    CHECK(&right_plane != &top_plane);
    CHECK(&right_plane != &bottom_plane);
    CHECK(&right_plane != &near_plane);
    CHECK(&right_plane != &far_plane);
    CHECK(&top_plane != &bottom_plane);
    CHECK(&top_plane != &near_plane);
    CHECK(&top_plane != &far_plane);
    CHECK(&bottom_plane != &near_plane);
    CHECK(&bottom_plane != &far_plane);
    CHECK(&near_plane != &far_plane);
  }

  TEST_CASE("intersects_behind_near_plane") {
    auto camera = Camera(1);
    auto& frustum = camera.get_frustum();
    auto box = BoundingBox(Point(-0.5f, -0.5f, -0.5f), Point(0.5f, 0.5f, 0.5f));
    CHECK(!intersects(frustum, box));
  }

  TEST_CASE("intersects_fully_contained") {
    auto camera = Camera(1);
    auto& frustum = camera.get_frustum();
    auto box = BoundingBox(Point(-0.1f, -0.1f, 2), Point(0.1f, 0.1f, 3));
    CHECK(intersects(frustum, box));
  }

  TEST_CASE("intersects_touch_near_plane") {
    auto camera = Camera(1);
    auto& frustum = camera.get_frustum();
    auto box = BoundingBox(Point(-0.5f, -0.5f, 1), Point(0.5f, 0.5f, 1.5f));
    CHECK(intersects(frustum, box));
  }

  TEST_CASE("intersects_straggling") {
    auto camera = Camera(1);
    auto& frustum = camera.get_frustum();
    auto box = BoundingBox(Point(-1, -0.5f, 1), Point(2, 0.5f, 3));
    CHECK(intersects(frustum, box));
  }

  TEST_CASE("near_plane_maps_to_screen") {
    auto width  = 800;
    auto height = 600;
    auto camera = Camera(static_cast<float>(width) / height);
    auto near_distance = -camera.get_near_plane();
    auto tan_half_fov = std::tan(camera.get_field_of_view() / 2);
    auto near_plane_half_height = tan_half_fov * near_distance;
    auto near_plane_half_width =
      near_plane_half_height * camera.get_aspect_ratio();
    auto eye = camera.get_position();
    auto forward = camera.get_direction();
    auto up = camera.get_orientation();
    auto right = camera.get_right();
    auto near_plane_top_left = eye + near_distance * forward +
      near_plane_half_height * up - near_plane_half_width * right;
    auto near_plane_top_right = eye + near_distance * forward +
      near_plane_half_height * up + near_plane_half_width * right;
    auto near_plane_bottom_right = eye + near_distance * forward -
      near_plane_half_height * up + near_plane_half_width * right;
    auto near_plane_bottom_left = eye + near_distance * forward -
      near_plane_half_height * up - near_plane_half_width * right;
    auto sc_tl = project_to_screen(
      world_to_view(near_plane_top_left, camera), camera, width, height);
    auto sc_tr = project_to_screen(
      world_to_view(near_plane_top_right, camera), camera, width, height);
    auto sc_br = project_to_screen(
      world_to_view(near_plane_bottom_right, camera), camera, width, height);
    auto sc_bl = project_to_screen(
      world_to_view(near_plane_bottom_left, camera), camera, width, height);
    CHECK(sc_tl.m_x == 0);
    CHECK(sc_tl.m_y == 0);
    CHECK(sc_tr.m_x == width  - 1);
    CHECK(sc_tr.m_y == 0);
    CHECK(sc_br.m_x == width  - 1);
    CHECK(sc_br.m_y == height - 1);
    CHECK(sc_bl.m_x == 0);
    CHECK(sc_bl.m_y == height - 1);
  }
}
