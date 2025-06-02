#ifndef ASHKAL_MESH_NODE_HPP
#define ASHKAL_MESH_NODE_HPP
#include <cstdint>
#include <variant>
#include <vector>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/MeshTriangle.hpp"

namespace Ashkal {
  class MeshNode {
    public:
      enum class Type {
        TRIANGLES,
        CHUNK
      };

      explicit MeshNode(std::vector<MeshTriangle> triangles);

      explicit MeshNode(std::vector<MeshNode> children);

      Type get_type() const;

      const std::vector<MeshTriangle>& as_triangles() const;

      std::vector<MeshTriangle>& as_triangles();

      const std::vector<MeshNode>& as_chunk() const;

      std::vector<MeshNode>& as_chunk();

    private:
      using Contents =
        std::variant<std::vector<MeshTriangle>, std::vector<MeshNode>>;
      Contents m_contents;
  };

  inline MeshNode::MeshNode(std::vector<MeshTriangle> triangles)
    : m_contents(std::move(triangles)) {}

  inline MeshNode::MeshNode(std::vector<MeshNode> children)
    : m_contents(std::move(children)) {}

  inline MeshNode::Type MeshNode::get_type() const {
    return static_cast<Type>(m_contents.index());
  }

  inline const std::vector<MeshTriangle>& MeshNode::as_triangles() const {
    return std::get<std::vector<MeshTriangle>>(m_contents);
  }

  inline std::vector<MeshTriangle>& MeshNode::as_triangles() {
    return std::get<std::vector<MeshTriangle>>(m_contents);
  }

  inline const std::vector<MeshNode>& MeshNode::as_chunk() const {
    return std::get<std::vector<MeshNode>>(m_contents);
  }

  std::vector<MeshNode>& MeshNode::as_chunk() {
    return std::get<std::vector<MeshNode>>(m_contents);
  }
}

#endif
