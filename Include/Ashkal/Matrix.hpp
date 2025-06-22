#ifndef ASHKAL_MATRIX_HPP
#define ASHKAL_MATRIX_HPP
#include <array>
#include <cmath>
#include <ostream>
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Represents a 4x4 matrix. */
  class Matrix {
    public:

      /** The number of columns in the matrix. */
      static constexpr auto WIDTH = 4;

      /** The number of rows in the matrix. */
      static constexpr auto HEIGHT = 4;

      /** Returns the identity matrix. */
      static const Matrix& IDENTITY();

      /** Returns the component at a specified index. */
      float get(int x, int y) const;

      /** Sets the component at a specified index. */
      void set(int x, int y, float value);

    private:
      std::array<float, WIDTH * HEIGHT> m_elements;

      friend Matrix invert(const Matrix& matrix);
      friend Matrix operator +(Matrix left, const Matrix& right);
      friend Matrix operator -(Matrix left, const Matrix& right);
  };

  /** Computes the inverse of a matrix. */
  inline Matrix invert(const Matrix& matrix) {
    auto inverse = Matrix();
    inverse.m_elements[0] =
      matrix.m_elements[5] * matrix.m_elements[10] * matrix.m_elements[15] -
      matrix.m_elements[5] * matrix.m_elements[11] * matrix.m_elements[14] -
      matrix.m_elements[9] * matrix.m_elements[6] * matrix.m_elements[15] +
      matrix.m_elements[9] * matrix.m_elements[7] * matrix.m_elements[14] +
      matrix.m_elements[13] * matrix.m_elements[6] * matrix.m_elements[11] -
      matrix.m_elements[13] * matrix.m_elements[7] * matrix.m_elements[10];
    inverse.m_elements[4] =
      -matrix.m_elements[4] * matrix.m_elements[10] * matrix.m_elements[15] +
      matrix.m_elements[4] * matrix.m_elements[11] * matrix.m_elements[14] +
      matrix.m_elements[8] * matrix.m_elements[6] * matrix.m_elements[15] -
      matrix.m_elements[8] * matrix.m_elements[7] * matrix.m_elements[14] -
      matrix.m_elements[12] * matrix.m_elements[6] * matrix.m_elements[11] +
      matrix.m_elements[12] * matrix.m_elements[7] * matrix.m_elements[10];
    inverse.m_elements[8] =
      matrix.m_elements[4] * matrix.m_elements[9] * matrix.m_elements[15] -
      matrix.m_elements[4] * matrix.m_elements[11] * matrix.m_elements[13] -
      matrix.m_elements[8] * matrix.m_elements[5] * matrix.m_elements[15] +
      matrix.m_elements[8] * matrix.m_elements[7] * matrix.m_elements[13] +
      matrix.m_elements[12] * matrix.m_elements[5] * matrix.m_elements[11] -
      matrix.m_elements[12] * matrix.m_elements[7] * matrix.m_elements[9];
    inverse.m_elements[12] =
      -matrix.m_elements[4] * matrix.m_elements[9] * matrix.m_elements[14] +
      matrix.m_elements[4] * matrix.m_elements[10] * matrix.m_elements[13] +
      matrix.m_elements[8] * matrix.m_elements[5] * matrix.m_elements[14] -
      matrix.m_elements[8] * matrix.m_elements[6] * matrix.m_elements[13] -
      matrix.m_elements[12] * matrix.m_elements[5] * matrix.m_elements[10] +
      matrix.m_elements[12] * matrix.m_elements[6] * matrix.m_elements[9];
    inverse.m_elements[1] =
      -matrix.m_elements[1] * matrix.m_elements[10] * matrix.m_elements[15] +
      matrix.m_elements[1] * matrix.m_elements[11] * matrix.m_elements[14] +
      matrix.m_elements[9] * matrix.m_elements[2] * matrix.m_elements[15] -
      matrix.m_elements[9] * matrix.m_elements[3] * matrix.m_elements[14] -
      matrix.m_elements[13] * matrix.m_elements[2] * matrix.m_elements[11] +
      matrix.m_elements[13] * matrix.m_elements[3] * matrix.m_elements[10];
    inverse.m_elements[5] =
      matrix.m_elements[0] * matrix.m_elements[10] * matrix.m_elements[15] -
      matrix.m_elements[0] * matrix.m_elements[11] * matrix.m_elements[14] -
      matrix.m_elements[8] * matrix.m_elements[2] * matrix.m_elements[15] +
      matrix.m_elements[8] * matrix.m_elements[3] * matrix.m_elements[14] +
      matrix.m_elements[12] * matrix.m_elements[2] * matrix.m_elements[11] -
      matrix.m_elements[12] * matrix.m_elements[3] * matrix.m_elements[10];
    inverse.m_elements[9] =
      -matrix.m_elements[0] * matrix.m_elements[9] * matrix.m_elements[15] +
      matrix.m_elements[0] * matrix.m_elements[11] * matrix.m_elements[13] +
      matrix.m_elements[8] * matrix.m_elements[1] * matrix.m_elements[15] -
      matrix.m_elements[8] * matrix.m_elements[3] * matrix.m_elements[13] -
      matrix.m_elements[12] * matrix.m_elements[1] * matrix.m_elements[11] +
      matrix.m_elements[12] * matrix.m_elements[3] * matrix.m_elements[9];
    inverse.m_elements[13] =
      matrix.m_elements[0] * matrix.m_elements[9] * matrix.m_elements[14] -
      matrix.m_elements[0] * matrix.m_elements[10] * matrix.m_elements[13] -
      matrix.m_elements[8] * matrix.m_elements[1] * matrix.m_elements[14] +
      matrix.m_elements[8] * matrix.m_elements[2] * matrix.m_elements[13] +
      matrix.m_elements[12] * matrix.m_elements[1] * matrix.m_elements[10] -
      matrix.m_elements[12] * matrix.m_elements[2] * matrix.m_elements[9];
    inverse.m_elements[2] =
      matrix.m_elements[1] * matrix.m_elements[6] * matrix.m_elements[15] -
      matrix.m_elements[1] * matrix.m_elements[7] * matrix.m_elements[14] -
      matrix.m_elements[5] * matrix.m_elements[2] * matrix.m_elements[15] +
      matrix.m_elements[5] * matrix.m_elements[3] * matrix.m_elements[14] +
      matrix.m_elements[13] * matrix.m_elements[2] * matrix.m_elements[7] -
      matrix.m_elements[13] * matrix.m_elements[3] * matrix.m_elements[6];
    inverse.m_elements[6] =
      -matrix.m_elements[0] * matrix.m_elements[6] * matrix.m_elements[15] +
      matrix.m_elements[0] * matrix.m_elements[7] * matrix.m_elements[14] +
      matrix.m_elements[4] * matrix.m_elements[2] * matrix.m_elements[15] -
      matrix.m_elements[4] * matrix.m_elements[3] * matrix.m_elements[14] -
      matrix.m_elements[12] * matrix.m_elements[2] * matrix.m_elements[7] +
      matrix.m_elements[12] * matrix.m_elements[3] * matrix.m_elements[6];
    inverse.m_elements[10] =
      matrix.m_elements[0] * matrix.m_elements[5] * matrix.m_elements[15] -
      matrix.m_elements[0] * matrix.m_elements[7] * matrix.m_elements[13] -
      matrix.m_elements[4] * matrix.m_elements[1] * matrix.m_elements[15] +
      matrix.m_elements[4] * matrix.m_elements[3] * matrix.m_elements[13] +
      matrix.m_elements[12] * matrix.m_elements[1] * matrix.m_elements[7] -
      matrix.m_elements[12] * matrix.m_elements[3] * matrix.m_elements[5];
    inverse.m_elements[14] =
      -matrix.m_elements[0] * matrix.m_elements[5] * matrix.m_elements[14] +
      matrix.m_elements[0] * matrix.m_elements[6] * matrix.m_elements[13] +
      matrix.m_elements[4] * matrix.m_elements[1] * matrix.m_elements[14] -
      matrix.m_elements[4] * matrix.m_elements[2] * matrix.m_elements[13] -
      matrix.m_elements[12] * matrix.m_elements[1] * matrix.m_elements[6] +
      matrix.m_elements[12] * matrix.m_elements[2] * matrix.m_elements[5];
    inverse.m_elements[3] =
      -matrix.m_elements[1] * matrix.m_elements[6] * matrix.m_elements[11] +
      matrix.m_elements[1] * matrix.m_elements[7] * matrix.m_elements[10] +
      matrix.m_elements[5] * matrix.m_elements[2] * matrix.m_elements[11] -
      matrix.m_elements[5] * matrix.m_elements[3] * matrix.m_elements[10] -
      matrix.m_elements[9] * matrix.m_elements[2] * matrix.m_elements[7] +
      matrix.m_elements[9] * matrix.m_elements[3] * matrix.m_elements[6];
    inverse.m_elements[7] =
      matrix.m_elements[0] * matrix.m_elements[6] * matrix.m_elements[11] -
      matrix.m_elements[0] * matrix.m_elements[7] * matrix.m_elements[10] -
      matrix.m_elements[4] * matrix.m_elements[2] * matrix.m_elements[11] +
      matrix.m_elements[4] * matrix.m_elements[3] * matrix.m_elements[10] +
      matrix.m_elements[8] * matrix.m_elements[2] * matrix.m_elements[7] -
      matrix.m_elements[8] * matrix.m_elements[3] * matrix.m_elements[6];
    inverse.m_elements[11] =
      -matrix.m_elements[0] * matrix.m_elements[5] * matrix.m_elements[11] +
      matrix.m_elements[0] * matrix.m_elements[7] * matrix.m_elements[9] +
      matrix.m_elements[4] * matrix.m_elements[1] * matrix.m_elements[11] -
      matrix.m_elements[4] * matrix.m_elements[3] * matrix.m_elements[9] -
      matrix.m_elements[8] * matrix.m_elements[1] * matrix.m_elements[7] +
      matrix.m_elements[8] * matrix.m_elements[3] * matrix.m_elements[5];
    inverse.m_elements[15] =
      matrix.m_elements[0] * matrix.m_elements[5] * matrix.m_elements[10] -
      matrix.m_elements[0] * matrix.m_elements[6] * matrix.m_elements[9] -
      matrix.m_elements[4] * matrix.m_elements[1] * matrix.m_elements[10] +
      matrix.m_elements[4] * matrix.m_elements[2] * matrix.m_elements[9] +
      matrix.m_elements[8] * matrix.m_elements[1] * matrix.m_elements[6] -
      matrix.m_elements[8] * matrix.m_elements[2] * matrix.m_elements[5];
    auto determinant =
      matrix.m_elements[0] * inverse.m_elements[0] + matrix.m_elements[1] *
        inverse.m_elements[4] + matrix.m_elements[2] * inverse.m_elements[8] +
          matrix.m_elements[3] * inverse.m_elements[12];
    determinant = 1.0f / determinant;
    for(auto& element : inverse.m_elements) {
      element *= determinant;
    }
    return inverse;
  }

  /**
   * Adds each element of the right matrix to the corresponding element of the
   * left matrix.
   * @param left The left-hand operand.
   * @param right The right-hand operand.
   * @return A new Matrix containing the element-wise sum.
   */
  inline Matrix operator +(Matrix left, const Matrix& right) {
    for(auto i = std::size_t(0); i != left.m_elements.size(); ++i) {
      left.m_elements[i] += right.m_elements[i];
    }
    return left;
  }


  /**
   * Subtracts each element of the right matrix from the corresponding element
   * of the left matrix.
   * @param left The left-hand operand.
   * @param right The right-hand operand.
   * @return A new Matrix containing the element-wise difference.
   */
  inline Matrix operator -(Matrix left, const Matrix& right) {
    for(auto i = std::size_t(0); i != left.m_elements.size(); ++i) {
      left.m_elements[i] -= right.m_elements[i];
    }
    return left;
  }

  /**
   * Computes the product of two matrices.
   * @param left The left-hand operand.
   * @param right The right-hand operand.
   * @return A new Matrix containing the product.
   */
  inline Matrix operator *(const Matrix& left, const Matrix& right) {
    auto result = Matrix();
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        auto e = 0.f;
        for(auto z = 0; z != Matrix::HEIGHT; ++z) {
          e += left.get(z, y) * right.get(x, z);
        }
        result.set(x, y, e);
      }
    }
    return result;
  }

  /**
   * Transforms a point by a matrix using homogeneous coordinates.
   * @param left The transformation matrix.
   * @param right The point to transform.
   * @return The transformed Point.
   */
  inline Point operator *(const Matrix& left, const Point& right) {
    auto get = [&] (int x) {
      if(x == 0) {
        return right.m_x;
      } else if(x == 1) {
        return right.m_y;
      } else if(x == 2) {
        return right.m_z;
      }
      return 1.f;
    };
    auto result = Point();
    auto set = [&] (int x, float v) {
      if(x == 0) {
        result.m_x = v;
      } else if(x == 1) {
        result.m_y = v;
      } else if(x == 2) {
        result.m_z = v;
      }
    };
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      auto e = 0.f;
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        e += left.get(x, y) * get(x);
      }
      set(y, e);
    }
    return result;
  }

  /**
   * Transforms a vector by a matrix (ignoring translation).
   * @param left The transformation matrix.
   * @param right The vector to transform.
   * @return The transformed Vector.
   */
  inline Vector operator *(const Matrix& left, const Vector& right) {
    auto result = Vector();
    auto get = [&] (auto x) {
      if(x == 0) {
        return right.m_x;
      } else if(x == 1) {
        return right.m_y;
      } else if(x == 2) {
        return right.m_z;
      }
      return 0.f;
    };
    auto set = [&] (auto x, auto v) {
      if(x == 0) {
        result.m_x = v;
      } else if(x == 1) {
        result.m_y = v;
      } else if(x == 2) {
        result.m_z = v;
      }
    };
    for(auto y = 0; y < Matrix::HEIGHT; ++y) {
      auto e = 0.f;
      for(auto x = 0; x < Matrix::WIDTH; ++x) {
        e += left.get(x, y) * get(x);
      }
      set(y, e);
    }
    return result;
  }

  /**
   * Constructs a translation matrix.
   * @param offset The translation vector.
   * @return A Matrix representing the translation.
   */
  inline Matrix translate(Vector offset) {
    auto translation = Matrix::IDENTITY();
    translation.set(3, 0, offset.m_x);
    translation.set(3, 1, offset.m_y);
    translation.set(3, 2, offset.m_z);
    return translation;
  }

  /**
   * Constructs a rotation matrix about an arbitrary axis.
   * @param axis The axis of rotation (should be normalized).
   * @param radians The rotation angle in radians.
   * @return A Matrix representing the rotation.
   */
  inline Matrix rotate(const Vector& axis, float radians) {
    auto x = axis.m_x;
    auto y = axis.m_y;
    auto z = axis.m_z;
    auto c = std::cos(radians);
    auto s = std::sin(radians);
    auto t = 1.0f - c;
    auto transform = Matrix::IDENTITY();
    transform.set(0, 0, t * x * x + c);
    transform.set(0, 1, t * x * y - s * z);
    transform.set(0, 2, t * x * z + s * y);
    transform.set(1, 0, t * x * y + s * z);
    transform.set(1, 1, t * y * y + c);
    transform.set(1, 2, t * y * z - s * x);
    transform.set(2, 0, t * x * z - s * y);
    transform.set(2, 1, t * y * z + s * x);
    transform.set(2, 2, t * z * z + c);
    return transform;
  }

  /**
   * Constructs a pitch rotation matrix (rotation about X-axis).
   * @param radians The rotation angle in radians.
   * @return A Matrix representing the pitch rotation.
   */
  inline Matrix pitch(float radians) {
    auto transform = Matrix::IDENTITY();
    transform.set(1, 1, std::cos(radians));
    transform.set(2, 1, -std::sin(radians));
    transform.set(1, 2, std::sin(radians));
    transform.set(2, 2, std::cos(radians));
    return transform;
  }

  /**
   * Constructs a yaw rotation matrix (rotation about Y-axis).
   * @param radians The rotation angle in radians.
   * @return A Matrix representing the yaw rotation.
   */
  inline Matrix yaw(float radians) {
    auto transform = Matrix::IDENTITY();
    transform.set(0, 0, std::cos(radians));
    transform.set(0, 2, std::sin(radians));
    transform.set(2, 0, -std::sin(radians));
    transform.set(2, 2, std::cos(radians));
    return transform;
  }

  /**
   * Constructs a roll rotation matrix (rotation about Z-axis).
   * @param radians The rotation angle in radians.
   * @return A Matrix representing the roll rotation.
   */
  inline Matrix roll(float radians) {
    auto transform = Matrix::IDENTITY();
    transform.set(0, 0, std::cos(radians));
    transform.set(0, 1, -std::sin(radians));
    transform.set(1, 0, std::sin(radians));
    transform.set(1, 1, std::cos(radians));
    return transform;
  }

  /**
   * Constructs a scaling matrix along the X-axis.
   * @param factor Scale factor along the X direction.
   * @return A Matrix representing the scaling.
   */
  inline Matrix scale_x(float factor) {
    auto scale = Matrix::IDENTITY();
    scale.set(0, 0, factor);
    return scale;
  }

  /**
   * Constructs a scaling matrix along the Y-axis.
   * @param factor Scale factor along the Y direction.
   * @return A Matrix representing the scaling.
   */
  inline Matrix scale_y(float factor) {
    auto scale = Matrix::IDENTITY();
    scale.set(1, 1, factor);
    return scale;
  }

  /**
   * Constructs a scaling matrix along the Z-axis.
   * @param factor Scale factor along the Z direction.
   * @return A Matrix representing the scaling.
   */
  inline Matrix scale_z(float factor) {
    auto scale = Matrix::IDENTITY();
    scale.set(2, 2, factor);
    return scale;
  }

  /**
   * Constructs a uniform scaling matrix.
   * @param factor Uniform scale factor for all three axes.
   * @return A Matrix representing the uniform scaling.
   */
  inline Matrix scale(float factor) {
    auto scale = Matrix::IDENTITY();
    scale.set(0, 0, factor);
    scale.set(1, 1, factor);
    scale.set(2, 2, factor);
    return scale;
  }

  /**
   * Transforms a vector by a matrix, applying only the linear part of the
   * transformation and ignoring translation.
   * @param transformation The transformation matrix to apply.
   * @param vector The vector to transform.
   * @return The transformed and vector.
   */
  inline Vector linear_transform(
      const Matrix& transformation, const Vector& vector) {
    auto transformed_vector = Vector();
    transformed_vector.m_x = transformation.get(0, 0) * vector.m_x +
      transformation.get(0, 1) * vector.m_y +
      transformation.get(0, 2) * vector.m_z;
    transformed_vector.m_y = transformation.get(1, 0) * vector.m_x +
      transformation.get(1, 1) * vector.m_y +
      transformation.get(1, 2) * vector.m_z;
    transformed_vector.m_z = transformation.get(2, 0) * vector.m_x +
      transformation.get(2, 1) * vector.m_y +
      transformation.get(2, 2) * vector.m_z;
    return transformed_vector;
  }

  inline std::ostream& operator <<(std::ostream& out, const Matrix& matrix) {
    out << "Matrix(";
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      if(y != 0) {
        out << ", ";
      }
      out << '(';
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        if(x != 0) {
          out << ", ";
        }
        out << matrix.get(x, y);
      }
      out << ')';
    }
    out << ')';
    return out;
  }

  inline const Matrix& Matrix::IDENTITY() {
    static auto identity = [] {
      auto identity = Matrix();
      identity.set(0, 0, 1);
      identity.set(1, 1, 1);
      identity.set(2, 2, 1);
      identity.set(3, 3, 1);
      return identity;
    }();
    return identity;
  }

  inline float Matrix::get(int x, int y) const {
    return m_elements[x + WIDTH * y];
  }

  inline void Matrix::set(int x, int y, float value) {
    m_elements[x + WIDTH * y] = value;
  }
}

#endif
