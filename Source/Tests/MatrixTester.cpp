#include <numbers>
#include <doctest/doctest.h>
#include "Ashkal/Matrix.hpp"

using namespace Ashkal;

TEST_SUITE("Matrix") {
  TEST_CASE("identity") {
    auto& identity = Matrix::IDENTITY();
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        if(i == j) {
          CHECK(identity.get(i, j) == 1.f);
        } else {
          CHECK(identity.get(i, j) == 0.f);
        }
      }
    }
  }

  TEST_CASE("get_and_set") {
    auto m = Matrix();
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        m.set(i, j, static_cast<float>(i * Matrix::WIDTH + j));
      }
    }
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        CHECK(m.get(i, j) == static_cast<float>(i * Matrix::WIDTH + j));
      }
    }
  }

  TEST_CASE("addition") {
    auto m1 = Matrix();
    auto m2 = Matrix();
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        auto value = static_cast<float>(i * Matrix::WIDTH + j);
        m1.set(i, j, value);
        m2.set(i, j, value);
      }
    }
    auto sum = m1 + m2;
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        CHECK(sum.get(i, j) == m1.get(i, j) + m2.get(i, j));
      }
    }
  }

  TEST_CASE("subtraction") {
    auto m1 = Matrix();
    auto m2 = Matrix();
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        auto value = static_cast<float>(i * Matrix::WIDTH + j);
        m1.set(i, j, value);
        m2.set(i, j, value);
      }
    }
    auto difference = m1 - m2;
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        CHECK(difference.get(i, j) == 0.f);
      }
    }
  }

  TEST_CASE("multiplication") {
    auto m1 = Matrix();
    m1.set(0, 0, 1.f);
    m1.set(0, 1, 2.f);
    m1.set(0, 2, 3.f);
    m1.set(0, 3, 4.f);
    m1.set(1, 0, 5.f);
    m1.set(1, 1, 6.f);
    m1.set(1, 2, 7.f);
    m1.set(1, 3, 8.f);
    m1.set(2, 0, 9.f);
    m1.set(2, 1, 10.f);
    m1.set(2, 2, 11.f);
    m1.set(2, 3, 12.f);
    m1.set(3, 0, 13.f);
    m1.set(3, 1, 14.f);
    m1.set(3, 2, 15.f);
    m1.set(3, 3, 16.f);
    auto m2 = Matrix();
    m2.set(0, 0, 17.f);
    m2.set(0, 1, 18.f);
    m2.set(0, 2, 19.f);
    m2.set(0, 3, 20.f);
    m2.set(1, 0, 21.f);
    m2.set(1, 1, 22.f);
    m2.set(1, 2, 23.f);
    m2.set(1, 3, 24.f);
    m2.set(2, 0, 25.f);
    m2.set(2, 1, 26.f);
    m2.set(2, 2, 27.f);
    m2.set(2, 3, 28.f);
    m2.set(3, 0, 29.f);
    m2.set(3, 1, 30.f);
    m2.set(3, 2, 31.f);
    m2.set(3, 3, 32.f);
    auto expected = Matrix();
    expected.set(0, 0, 538.f);
    expected.set(0, 1, 612.f);
    expected.set(0, 2, 686.f);
    expected.set(0, 3, 760.f);
    expected.set(1, 0, 650.f);
    expected.set(1, 1, 740.f);
    expected.set(1, 2, 830.f);
    expected.set(1, 3, 920.f);
    expected.set(2, 0, 762.f);
    expected.set(2, 1, 868.f);
    expected.set(2, 2, 974.f);
    expected.set(2, 3, 1080.f);
    expected.set(3, 0, 874.f);
    expected.set(3, 1, 996.f);
    expected.set(3, 2, 1118.f);
    expected.set(3, 3, 1240.f);
    auto result = m1 * m2;
    auto epsilon = std::numeric_limits<float>::epsilon();
    for(auto i = 0; i < Matrix::HEIGHT; ++i) {
      for(auto j = 0; j < Matrix::WIDTH; ++j) {
        CHECK(result.get(i, j) == doctest::Approx(
          expected.get(i, j)).epsilon(epsilon));
      }
    }
  }

  TEST_CASE("point_multiplication") {
    auto& identity = Matrix::IDENTITY();
    auto p = Point(1.f, 2.f, 3.f);
    auto result = identity * p;
    CHECK(result == p);
  }

  TEST_CASE("vector_multiplication") {
    auto& identity = Matrix::IDENTITY();
    auto v = Vector(1.f, 2.f, 3.f);
    auto result = identity * v;
    CHECK(result == v);
  }

  TEST_CASE("translation") {
    auto offset = Vector(1.f, 2.f, 3.f);
    auto translation = translate(offset);
    auto p = Point(1.f, 1.f, 1.f);
    auto result = translation * p;
    CHECK(result == p + offset);
  }

  TEST_CASE("pitch") {
    auto radians = std::numbers::pi_v<float> / 2;
    auto pitched = pitch(radians);
    auto v = Vector(0.f, 1.f, 0.f);
    auto result = pitched * v;
    auto epsilon = std::numeric_limits<float>::epsilon();
    CHECK(result.m_x == doctest::Approx(0.f).epsilon(epsilon));
    CHECK(result.m_y == doctest::Approx(0.f).epsilon(epsilon));
    CHECK(result.m_z == doctest::Approx(1.f).epsilon(epsilon));
  }

  TEST_CASE("yaw") {
    {
      auto radians = std::numbers::pi_v<float> / 2;
      auto yawed = yaw(radians);
      auto v = Vector(1.f, 0.f, 0.f);
      auto result = yawed * v;
      auto epsilon = std::numeric_limits<float>::epsilon();
      CHECK(result.m_x == doctest::Approx(0.f).epsilon(epsilon));
      CHECK(result.m_y == doctest::Approx(0.f));
      CHECK(result.m_z == doctest::Approx(1.f).epsilon(epsilon));
    }
    auto radians = std::numbers::pi_v<float> / 4;
    auto yawed = yaw(radians);
    auto v = Vector(1.f, 0.f, 0.f);
    auto result = yawed * v;
    auto epsilon = std::numeric_limits<float>::epsilon();
    CHECK(result.m_x == doctest::Approx(
      cos(radians) * v.m_x - sin(radians) * v.m_z).epsilon(epsilon));
    CHECK(result.m_y == doctest::Approx(v.m_y));
    CHECK(result.m_z == doctest::Approx(
      sin(radians) * v.m_x + cos(radians) * v.m_z).epsilon(epsilon));
  }

  TEST_CASE("invert_identity") {
    auto identity = Matrix::IDENTITY();
    auto inverted_identity = invert(identity);
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        if(i == j) {
          CHECK(inverted_identity.get(i, j) == doctest::Approx(1.f));
        } else {
          CHECK(inverted_identity.get(i, j) == doctest::Approx(0.f));
        }
      }
    }
  }

  TEST_CASE("invert_translation") {
    auto translation_matrix = translate(Vector(10, -5, 2));
    auto inverted_translation = invert(translation_matrix);
    auto expected_inverted_translation = translate(Vector(-10, 5, -2));
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        CHECK(inverted_translation.get(i, j) ==
          doctest::Approx(expected_inverted_translation.get(i, j)));
      }
    }
  }

  TEST_CASE("invert_and_multiply") {
    auto matrix_a = Matrix();
    matrix_a.set(0, 0, 2);
    matrix_a.set(1, 1, 3);
    matrix_a.set(2, 2, 4);
    matrix_a.set(3, 3, 1);
    auto inverse_a = invert(matrix_a);
    auto product = matrix_a * inverse_a;
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        if(i == j) {
          CHECK(product.get(i, j) == doctest::Approx(1.f));
        } else {
          CHECK(product.get(i, j) == doctest::Approx(0.f));
        }
      }
    }
  }
}
