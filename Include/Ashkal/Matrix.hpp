#ifndef ASHKAL_MATRIX_HPP
#define ASHKAL_MATRIX_HPP
#include <array>
#include <cmath>
#include <ostream>
#include "Ashkal/Ashkal.hpp"
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

  inline Matrix operator +(Matrix left, const Matrix& right) {
    for(auto i = std::size_t(0); i != left.m_elements.size(); ++i) {
      left.m_elements[i] += right.m_elements[i];
    }
    return left;
  }

  inline Matrix operator -(Matrix left, const Matrix& right) {
    for(auto i = std::size_t(0); i != left.m_elements.size(); ++i) {
      left.m_elements[i] -= right.m_elements[i];
    }
    return left;
  }

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

  inline Vector operator *(const Matrix& left, const Vector& right) {
    auto result = Vector();
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

  inline Matrix translate(Vector offset) {
    auto translation = Matrix::IDENTITY();
    translation.set(3, 0, offset.m_x);
    translation.set(3, 1, offset.m_y);
    translation.set(3, 2, offset.m_z);
    return translation;
  }

  inline Matrix pitch(float radians) {
    auto transform = Matrix::IDENTITY();
    transform.set(1, 1, std::cos(radians));
    transform.set(2, 1, -std::sin(radians));
    transform.set(1, 2, std::sin(radians));
    transform.set(2, 2, std::cos(radians));
    return transform;
  }

  inline Matrix yaw(float radians) {
    auto transform = Matrix::IDENTITY();
    transform.set(0, 0, std::cos(radians));
    transform.set(0, 2, std::sin(radians));
    transform.set(2, 0, -std::sin(radians));
    transform.set(2, 2, std::cos(radians));
    return transform;
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
