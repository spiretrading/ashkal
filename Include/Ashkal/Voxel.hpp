#ifndef ASHKAL_VOXEL_HPP
#define ASHKAL_VOXEL_HPP
#include <ostream>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"

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
}

#endif
