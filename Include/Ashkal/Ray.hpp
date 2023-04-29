#ifndef ASHKAL_RAY_HPP
#define ASHKAL_RAY_HPP
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  struct Ray {
    Point m_point;
    Vector m_direction;
  };

  inline std::ostream& operator <<(std::ostream& out, const Ray& ray) {
    return out << "Ray(" << ray.m_point << ", " << ray.m_direction << ')';
  }

  inline Point point_at(const Ray& ray, float t) {
    return ray.m_point + t * ray.m_direction;
  }

  inline Point compute_boundary(const Ray& ray, Point start, int size) {
    auto x_distance = [&] {
      if (ray.m_direction.m_x == 0) {
        return INFINITY;
      }
      if (ray.m_direction.m_x > 0) {
        return start.m_x + size - ray.m_point.m_x;
      }
      return start.m_x - ray.m_point.m_x - 1;
    }();
    auto y_distance = [&] {
      if (ray.m_direction.m_y == 0) {
        return INFINITY;
      }
      if (ray.m_direction.m_y > 0) {
        return start.m_y + size - ray.m_point.m_y;
      }
      return start.m_y - ray.m_point.m_y - 1;
    }();
    auto z_distance = [&] {
      if (ray.m_direction.m_z == 0) {
        return INFINITY;
      }
      if (ray.m_direction.m_z > 0) {
        return start.m_z + size - ray.m_point.m_z;
      }
      return start.m_z - ray.m_point.m_z - 1;
    }();
    auto t = INFINITY;
    if (x_distance != INFINITY) {
      t = x_distance / ray.m_direction.m_x;
    }
    if (y_distance != INFINITY) {
      auto r = y_distance / ray.m_direction.m_y;
      if (r < t) {
        t = r;
      }
    }
    if (z_distance != INFINITY) {
      auto r = z_distance / ray.m_direction.m_z;
      if (r < t) {
        t = r;
      }
    }
    return point_at(ray, t);
  }

  inline std::string RAY_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Point point_at(Ray ray, float t) {
      return add_point_vector(
        ray.m_point, mul_float_vector(t, ray.m_direction));
    }

    Point compute_boundary(Ray ray, Point start, int size) {
      float x_distance;
      if (ray.m_direction.m_x == 0) {
        x_distance = INFINITY;
      }
      else if (ray.m_direction.m_x > 0) {
        x_distance = start.m_x + size - ray.m_point.m_x;
      }
      else {
        x_distance = start.m_x - ray.m_point.m_x - 0.001f;
      }
      float y_distance;
      if (ray.m_direction.m_y == 0) {
        y_distance = INFINITY;
      }
      else if (ray.m_direction.m_y > 0) {
        y_distance = start.m_y + size - ray.m_point.m_y;
      }
      else {
        y_distance = start.m_y - ray.m_point.m_y - 0.001f;
      }
      float z_distance;
      if (ray.m_direction.m_z == 0) {
        z_distance = INFINITY;
      }
      else if (ray.m_direction.m_z > 0) {
        z_distance = start.m_z + size - ray.m_point.m_z;
      }
      else {
        z_distance = start.m_z - ray.m_point.m_z - 0.001f;
      }
      float t = INFINITY;
      if (x_distance != INFINITY) {
        t = x_distance / ray.m_direction.m_x;
      }
      if (y_distance != INFINITY) {
        float r = y_distance / ray.m_direction.m_y;
        if (r < t) {
          t = r;
        }
      }
      if (z_distance != INFINITY) {
        float r = z_distance / ray.m_direction.m_z;
        if (r < t) {
          t = r;
        }
      }
      return point_at(ray, t);
    });
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::Ray, Ray, (m_point, m_direction));

#endif
