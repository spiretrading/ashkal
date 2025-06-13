#ifndef ASHKAL_VECTOR_HPP
#define ASHKAL_VECTOR_HPP
#include <cmath>
#include <ostream>
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
}

#endif
