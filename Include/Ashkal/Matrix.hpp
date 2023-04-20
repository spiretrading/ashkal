#ifndef ASHKAL_MATRIX_HPP
#define ASHKAL_MATRIX_HPP
#include <array>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  class Matrix {
    public:
      static constexpr auto WIDTH = 4;
      static constexpr auto HEIGHT = 4;

      static const Matrix& IDENTITY();

      float get(int x, int y) const;

      void set(int x, int y, float value);

    private:
      std::array<float, WIDTH * HEIGHT> m_elements;

      friend Matrix operator +(Matrix left, const Matrix& right);
      friend Matrix operator -(Matrix left, const Matrix& right);
      friend Matrix operator *(Matrix left, const Matrix& right);
  };

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

  inline Matrix operator *(Matrix left, const Matrix& right) {
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        auto e = 0.f;
        for(auto z = 0; z != Matrix::HEIGHT; ++z) {
          e += left.get(z, y) * right.get(x, z);
        }
        left.set(x, y, e);
      }
    }
    return left;
  }

  inline Point operator *(const Matrix& left, Point right) {
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
        right.m_x = v;
      } else if(x == 1) {
        right.m_y = v;
      } else if(x == 2) {
        right.m_z = v;
      }
    };
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      auto e = 0.f;
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        e += left.get(x, y) * get(x);
      }
      set(y, e);
    }
    return right;
  }

  inline Vector operator *(const Matrix& left, Vector right) {
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
        right.m_x = v;
      } else if(x == 1) {
        right.m_y = v;
      } else if(x == 2) {
        right.m_z = v;
      }
    };
    for(auto y = 0; y != Matrix::HEIGHT; ++y) {
      auto e = 0.f;
      for(auto x = 0; x != Matrix::WIDTH; ++x) {
        e += left.get(x, y) * get(x);
      }
      set(y, e);
    }
    return right;
  }

  inline Matrix translate(Vector offset) {
    auto translation = Matrix::IDENTITY();
    translation.set(3, 0, offset.m_x);
    translation.set(3, 1, offset.m_y);
    translation.set(3, 2, offset.m_z);
    return translation;
  }

  inline Matrix pitch(float radians) {
    auto transform = Matrix();
    transform.set(0, 0, cos(radians));
    transform.set(1, 0, 0);
    transform.set(2, 0, sin(radians));
    transform.set(3, 0, 0);
    transform.set(0, 1, 0);
    transform.set(1, 1, 1);
    transform.set(2, 1, 0);
    transform.set(3, 1, 0);
    transform.set(0, 2, -sin(radians));
    transform.set(1, 2, 0);
    transform.set(2, 2, cos(radians));
    transform.set(3, 2, 0);
    transform.set(0, 3, 0);
    transform.set(1, 3, 0);
    transform.set(2, 3, 0);
    transform.set(3, 3, 1);
    return transform;
  }

  inline Matrix yaw(float radians) {
    auto transform = Matrix();
    transform.set(0, 0, 1);
    transform.set(1, 0, 0);
    transform.set(2, 0, 0);
    transform.set(3, 0, 0);
    transform.set(0, 1, 0);
    transform.set(1, 1, cos(radians));
    transform.set(2, 1, -sin(radians));
    transform.set(3, 1, 0);
    transform.set(0, 2, 0);
    transform.set(1, 2, sin(radians));
    transform.set(2, 2, cos(radians));
    transform.set(3, 2, 0);
    transform.set(0, 3, 0);
    transform.set(1, 3, 0);
    transform.set(2, 3, 0);
    transform.set(3, 3, 1);
    return transform;
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
