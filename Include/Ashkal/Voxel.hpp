#ifndef ASHKAL_VOXEL_HPP
#define ASHKAL_VOXEL_HPP
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Represents a voxel. */
  struct Voxel {

    /** Returns an empty voxel. */
    constexpr static Voxel NONE();

    /** The voxel's color. */
    Color m_color;

    friend auto operator <=>(const Voxel&, const Voxel&) = default;
  };

  constexpr Voxel Voxel::NONE() {
    return Voxel(Color(0, 0, 0, 255));
  }

  inline std::ostream& operator <<(std::ostream& out, Voxel voxel) {
    return out << "Voxel(" << voxel.m_color << ')';
  }

  inline Vector compute_surface_normal(Point position, Point intersection) {
    auto min_distance = std::abs(position.m_x - intersection.m_x);
    auto normal = Vector(-1.f, 0.f, 0.f);
    auto distance = std::abs(position.m_x + 1.f - intersection.m_x);
    if(distance < min_distance) {
      min_distance = distance;
      normal = Vector(1.f, 0.f, 0.f);
    }
    distance = std::abs(position.m_y - intersection.m_y);
    if(distance < min_distance) {
      min_distance = distance;
      normal = Vector(0.f, -1.f, 0.f);
    }
    distance = std::abs(position.m_y + 1.f - intersection.m_y);
    if(distance < min_distance) {
      min_distance = distance;
      normal = Vector(0.f, 1.f, 0.f);
    }
    distance = std::abs(position.m_z - intersection.m_z);
    if(distance < min_distance) {
      min_distance = distance;
      normal = Vector(0.f, 0.f, -1.f);
    }
    distance = std::abs(position.m_z + 1.f - intersection.m_z);
    if(distance < min_distance) {
      min_distance = distance;
      normal = Vector(0.f, 0.f, 1.f);
    }
    return normal;
  }

  inline std::string VOXEL_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Voxel make_voxel(Color color) {
      Voxel voxel;
      voxel.m_color = color;
      return voxel;
    }

    bool is_none_voxel(Voxel voxel) {
      return is_equal_color(voxel.m_color, make_color(0, 0, 0, 255));
    }

    Vector compute_surface_normal(Point position, Point intersection) {
      float min_distance = fabs(position.m_x - intersection.m_x);
      Vector normal = make_vector(-1.f, 0.f, 0.f);
      float distance = fabs(position.m_x + 1.f - intersection.m_x);
      if(distance < min_distance) {
        min_distance = distance;
        normal = make_vector(1.f, 0.f, 0.f);
      }
      distance = fabs(position.m_y - intersection.m_y);
      if(distance < min_distance) {
        min_distance = distance;
        normal = make_vector(0.f, -1.f, 0.f);
      }
      distance = fabs(position.m_y + 1.f - intersection.m_y);
      if(distance < min_distance) {
        min_distance = distance;
        normal = make_vector(0.f, 1.f, 0.f);
      }
      distance = fabs(position.m_z - intersection.m_z);
      if(distance < min_distance) {
        min_distance = distance;
        normal = make_vector(0.f, 0.f, -1.f);
      }
      distance = fabs(position.m_z + 1.f - intersection.m_z);
      if(distance < min_distance) {
        min_distance = distance;
        normal = make_vector(0.f, 0.f, 1.f);
      }
      return normal;
    }
  );
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::Voxel, Voxel, (m_color));

#endif
