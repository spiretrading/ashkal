#ifndef ASHKAL_MESH_HPP
#define ASHKAL_MESH_HPP
#include <vector>
#include "Ashkal/MeshNode.hpp"
#include "Ashkal/Vertex.hpp"

namespace Ashkal {
  struct Mesh {
    std::vector<Vertex> m_vertices;
    MeshNode m_root;
  };
}

#endif
