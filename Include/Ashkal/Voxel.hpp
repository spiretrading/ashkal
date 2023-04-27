#ifndef ASHKAL_VOXEL_HPP
#define ASHKAL_VOXEL_HPP
#include <ostream>
#include <string>
#include <boost/compute/types/struct.hpp>
#include <boost/compute/utility/source.hpp>
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

  inline std::string VOXEL_CL_SOURCE = BOOST_COMPUTE_STRINGIZE_SOURCE(
    Voxel make_voxel(Color color) {
      Voxel voxel;
      voxel.m_color = color;
      return voxel;
    }

    bool is_none_voxel(Voxel voxel) {
      return is_equal_color(voxel.m_color, make_color(0, 0, 0, 255));
    }
  );
}

BOOST_COMPUTE_ADAPT_STRUCT(Ashkal::Voxel, Voxel, (m_color));

#endif
