#ifndef ASHKAL_MESH_HPP
#define ASHKAL_MESH_HPP
#include <vector>
#include "Ashkal/MeshNode.hpp"
#include "Ashkal/Vertex.hpp"

namespace Ashkal {

  /**
   * A Mesh owns a flat list of Vertex instances and a root MeshNode that
   * organizes those vertices into Fragments or nested chunks for rendering.
   */
  struct Mesh {

    /** The vertices composing this mesh. */
    std::vector<Vertex> m_vertices;


    /** The root node defining the mesh's hierarchy. */
    MeshNode m_root;
  };
}

#endif
