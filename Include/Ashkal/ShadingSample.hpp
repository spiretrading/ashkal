#ifndef ASHKAL_SHADING_SAMPLE_HPP
#define ASHKAL_SHADING_SAMPLE_HPP
#include <ostream>
#include "Ashkal/Color.hpp"
#include "Ashkal/ShadingTerm.hpp"

namespace Ashkal {

  /** A single shading evaluation combining lighting and base color. */
  struct ShadingSample {

    /** The computed shading term. */
    ShadingTerm m_shading;

    /** The base color read or computed at the sample point. */
    Color m_color;
  };

  inline std::ostream& operator <<(
      std::ostream& out, const ShadingSample& sample) {
    return out << "ShadingSample(" << sample.m_shading << ", " <<
      sample.m_color << ")";
  }
}

#endif
