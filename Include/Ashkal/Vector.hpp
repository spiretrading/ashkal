#ifndef ASHKAL_VECTOR_HPP
#define ASHKAL_VECTOR_HPP
#include <cmath>
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Point.hpp"

namespace Ashkal {

  /** Stores a vector in voxel space. */
  struct Vector {

    /** The vector's x-component. */
    float m_x;

    /** The vector's y-component. */
    float m_y;

    /** The vector's z-component. */
    float m_z;

    /** Constructs a Vector at the origin. */
    Vector();

    /** Constructs a Vector component-wise. */
    Vector(float x, float y, float z);

    /** Constructs a Vector from the origin to a specified point. */
    explicit Vector(Point point);

    bool operator ==(const Vector&) const = default;

    bool operator !=(const Vector&) const = default;
  };

  inline std::ostream& operator <<(std::ostream& out, Vector vector) {
    return out << "Vector(" <<
      vector.m_x << ", " << vector.m_y << ", " << vector.m_z << ')';
  }

  inline Vector operator -(Vector vector) {
    return Vector(-vector.m_x, -vector.m_y, -vector.m_z);
  }

  inline Vector operator -(Point left, Point right) {
    return Vector(
      left.m_x - right.m_x, left.m_y - right.m_y, left.m_z - right.m_z);
  }

  inline Vector operator -(Vector left, Vector right) {
    return Vector(
      left.m_x - right.m_x, left.m_y - right.m_y, left.m_z - right.m_z);
  }

  inline Vector operator +(Vector left, Vector right) {
    return Vector(
      left.m_x + right.m_x, left.m_y + right.m_y, left.m_z + right.m_z);
  }

  inline Vector operator *(int left, Vector right) {
    return Vector(left * right.m_x, left * right.m_y, left * right.m_z);
  }

  inline Vector operator *(float left, Vector right) {
    return Vector(left * right.m_x, left * right.m_y, left * right.m_z);
  }

  inline Vector operator /(Vector left, int right) {
    return Vector(left.m_x / right, left.m_y / right, left.m_z / right);
  }

  inline Vector operator /(Vector left, float right) {
    return Vector(left.m_x / right, left.m_y / right, left.m_z / right);
  }

  inline Point operator +(Point left, Vector right) {
    return Point(
      left.m_x + right.m_x, left.m_y + right.m_y, left.m_z + right.m_z);
  }

  inline Point operator -(Point left, Vector right) {
    return left + -right;
  }

  inline Vector cross(Vector left, Vector right) {
    return Vector(left.m_y * right.m_z - left.m_z * right.m_y,
      left.m_z * right.m_x - left.m_x * right.m_z,
      left.m_x * right.m_y - left.m_y * right.m_x);
  }

  inline float dot(Vector left, Vector right) {
    return left.m_x * right.m_x + left.m_y * right.m_y + left.m_z * right.m_z;
  }

  inline float magnitude(Vector vector) {
    return std::sqrt(vector.m_x * vector.m_x +
      vector.m_y * vector.m_y + vector.m_z * vector.m_z);
  }

  inline Vector normalize(Vector vector) {
    return vector / magnitude(vector);
  }

  inline Vector::Vector()
    : m_x(0.f),
      m_y(0.f),
      m_z(0.f) {}

  inline Vector::Vector(float x, float y, float z)
    : m_x(x),
      m_y(y),
      m_z(z) {}

  inline Vector::Vector(Point point)
    : Vector(point.m_x, point.m_y, point.m_z) {}

  inline std::string VECTOR_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Vector make_vector(float x, float y, float z) {
      Vector vector;
      vector.m_x = x;
      vector.m_y = y;
      vector.m_z = z;
      return vector;
    }

    Vector point_to_vector(Point point) {
      return make_vector(point.m_x, point.m_y, point.m_z);
    }

    Vector add_vector(Vector left, Vector right) {
      left.m_x += right.m_x;
      left.m_y += right.m_y;
      left.m_z += right.m_z;
      return left;
    }

    Vector sub_vector(Vector left, Vector right) {
      left.m_x -= right.m_x;
      left.m_y -= right.m_y;
      left.m_z -= right.m_z;
      return left;
    }

    Vector mul_int_vector(int left, Vector right) {
      right.m_x *= left;
      right.m_y *= left;
      right.m_z *= left;
      return right;
    }

    Vector mul_float_vector(float left, Vector right) {
      right.m_x *= left;
      right.m_y *= left;
      right.m_z *= left;
      return right;
    }

    Vector div_vector_float(Vector left, float right) {
      left.m_x /= right;
      left.m_y /= right;
      left.m_z /= right;
      return left;
    }

    Point add_point_vector(Point point, Vector vector) {
      point.m_x += vector.m_x;
      point.m_y += vector.m_y;
      point.m_z += vector.m_z;
      return point;
    }

    Vector negate_vector(Vector vector) {
      return make_vector(-vector.m_x, -vector.m_y, -vector.m_z);
    }

    float vector_dot(Vector a, Vector b) {
      return a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z;
    }

    float magnitude(Vector vector) {
      return sqrt(vector.m_x * vector.m_x +
        vector.m_y * vector.m_y + vector.m_z * vector.m_z);
    }

    Vector normalize_vector(Vector vector) {
      return div_vector_float(vector, magnitude(vector));
    }

    Vector reverse_vector(Vector vector) {
      return make_vector(-vector.m_x, -vector.m_y, -vector.m_z);
    }

    Vector compute_direction_normal(Vector direction) {
      float abs_x = fabs(direction.m_x);
      float abs_y = fabs(direction.m_y);
      float abs_z = fabs(direction.m_z);
      if (abs_x >= abs_y && abs_x >= abs_z) {
        return make_vector(sign(direction.m_x), 0.f, 0.f);
      } else if (abs_y >= abs_x && abs_y >= abs_z) {
        return make_vector(0.f, sign(direction.m_y), 0.f);
      } else {
        return make_vector(0.f, 0.f, sign(direction.m_z));
      }
    }

    void print_vector(Vector vector) {
      printf("Vector(%f, %f, %f)", vector.m_x, vector.m_y, vector.m_z);
    }
  );
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::Vector, Vector, (m_x, m_y, m_z));

#endif
