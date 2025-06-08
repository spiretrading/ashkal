#ifndef ASHKAL_SHADING_SAMPLE_HPP
#define ASHKAL_SHADING_SAMPLE_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/ShadingTerm.hpp"

namespace Ashkal {
  struct ShadingSample {
    ShadingTerm m_shading;
    Color m_color;
  };
}

#endif
