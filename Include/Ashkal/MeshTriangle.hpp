#ifndef ASHKAL_MESH_TRIANGLE_HPP
#define ASHKAL_MESH_TRIANGLE_HPP
#include <cstdint>
#include "Ashkal/Ashkal.hpp"

namespace Ashkal {
  struct MeshTriangle {
    std::uint16_t m_a;
    std::uint16_t m_b;
    std::uint16_t m_c;
  };
}

#endif
