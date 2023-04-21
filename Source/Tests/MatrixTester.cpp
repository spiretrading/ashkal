#include <doctest/doctest.h>
#include "Ashkal/Matrix.hpp"

using namespace Ashkal;

TEST_SUITE("Matrix") {
  TEST_CASE("invert_identity") {
    auto identity = Matrix::IDENTITY();
    auto inverted_identity = invert(identity);
    for(auto i = 0; i < Matrix::WIDTH; ++i) {
      for(auto j = 0; j < Matrix::HEIGHT; ++j) {
        if(i == j) {
          CHECK(inverted_identity.get(i, j) == doctest::Approx(1.0f));
        } else {
          CHECK(inverted_identity.get(i, j) == doctest::Approx(0.0f));
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
          CHECK(product.get(i, j) == doctest::Approx(1.0f));
        } else {
          CHECK(product.get(i, j) == doctest::Approx(0.0f));
        }
      }
    }
  }
}
