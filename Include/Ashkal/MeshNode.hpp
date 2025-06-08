#ifndef ASHKAL_MESH_NODE_HPP
#define ASHKAL_MESH_NODE_HPP
#include <cstdint>
#include <variant>
#include <vector>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Fragment.hpp"

namespace Ashkal {
  class MeshNode {
    public:
      enum class Type {
        FRAGMENT,
        CHUNK
      };

      explicit MeshNode(Fragment fragment);

      explicit MeshNode(std::vector<MeshNode> children);

      Type get_type() const;

      const Fragment& as_fragment() const;

      const std::vector<MeshNode>& as_chunk() const;

    private:
      using Contents = std::variant<Fragment, std::vector<MeshNode>>;
      Contents m_contents;
  };

  inline MeshNode::MeshNode(Fragment fragment)
    : m_contents(std::move(fragment)) {}

  inline MeshNode::MeshNode(std::vector<MeshNode> children)
    : m_contents(std::move(children)) {}

  inline MeshNode::Type MeshNode::get_type() const {
    return static_cast<Type>(m_contents.index());
  }

  inline const Fragment& MeshNode::as_fragment() const {
    return std::get<Fragment>(m_contents);
  }

  inline const std::vector<MeshNode>& MeshNode::as_chunk() const {
    return std::get<std::vector<MeshNode>>(m_contents);
  }
}

#endif
