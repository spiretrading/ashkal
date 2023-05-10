#include <doctest/doctest.h>
#include <boost/compute/container/vector.hpp>
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/DirectionalLight.hpp"

using namespace Ashkal;
using namespace boost;

namespace {
  Color test_apply_directional_light(
      DirectionalLight light, Vector surface_normal, Color color) {
    auto test_context = compute::context(compute::system::default_device());
    auto kernel = [&] {
      auto source = compute::type_definition<Color>() +
        compute::type_definition<Point>() +
        compute::type_definition<Vector>() +
        compute::type_definition<AmbientLight>() +
        compute::type_definition<DirectionalLight>() +
        COLOR_CL_SOURCE +
        POINT_CL_SOURCE +
        VECTOR_CL_SOURCE +
        AMBIENT_LIGHT_CL_SOURCE +
        DIRECTIONAL_LIGHT_CL_SOURCE +
        BOOST_COMPUTE_STRINGIZE_SOURCE(
          __kernel void test(DirectionalLight light, Vector surface_normal,
              Color color, __global Color * result) {
            *result = apply_directional_light(light, surface_normal, color);
          });
      auto cache = compute::program_cache::get_global_cache(test_context);
      auto key = std::string("__test");
      auto program = cache->get_or_build(key, {}, source, test_context);
      return program.create_kernel("test");
    }();
    auto result = compute::vector<Color>(1, test_context);
    result.push_back(Color());
    kernel.set_arg(0, sizeof(DirectionalLight), &light);
    kernel.set_arg(1, sizeof(Vector), &surface_normal);
    kernel.set_arg(2, sizeof(Color), &color);
    kernel.set_arg(3, result.get_buffer());
    auto test_queue =
      compute::command_queue(test_context, test_context.get_device());
    test_queue.enqueue_task(kernel);
    test_queue.finish();
    return result.at(0);
  }
}

TEST_SUITE("DirectionalLight") {
  TEST_CASE("apply_directional_light") {
    auto light =
      DirectionalLight(Vector(0, 0, 1), Color(255, 255, 255, 0), 1.f);
    auto color = Color(255, 0, 0, 0);
    auto front_color =
      test_apply_directional_light(light, Vector(0, 0, -1), color);
    CHECK(front_color == Color(255, 0, 0, 0));
    auto back_color =
      test_apply_directional_light(light, Vector(0, 0, 1), color);
    CHECK(back_color == Color(0, 0, 0, 0));
    auto left_color =
      test_apply_directional_light(light, Vector(-1, 0, 0), color);
    CHECK(left_color == Color(0, 0, 0, 0));
    auto right_color =
      test_apply_directional_light(light, Vector(1, 0, 0), color);
    CHECK(right_color == Color(0, 0, 0, 0));
    auto top_color =
      test_apply_directional_light(light, Vector(0, 1, 0), color);
    CHECK(top_color == Color(0, 0, 0, 0));
    auto bottom_color =
      test_apply_directional_light(light, Vector(0, -1, 0), color);
    CHECK(bottom_color == Color(0, 0, 0, 0));
  }
}
