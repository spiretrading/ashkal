#ifndef ASHKAL_MESH_NODE_HPP
#define ASHKAL_MESH_NODE_HPP
#include <cstdint>
#include <variant>
#include <vector>
#include "Ashkal/Fragment.hpp"

namespace Ashkal {

  /**
   * A node in the mesh hierarchy, representing either a leaf Fragment or an
   * internal Chunk of child nodes.
   */
  class MeshNode {
    public:

      /** Discriminates the kind of content stored in this node. */
      enum class Type {

        /** Leaf node containing a single Fragment. */
        FRAGMENT,

        /** Internal node containing multiple child MeshNodes. */
        CHUNK
      };

      /**
       * Construct a leaf node holding a Fragment.
       * @param fragment The Fragment to store in this mesh node.
       */
      explicit MeshNode(Fragment fragment);


      /**
       * Construct an internal node holding a collection of children.
       * @param children The list of MeshNode instances to form this chunk.
       */
      explicit MeshNode(std::vector<MeshNode> children);

      /** Returns the content type of this node. */
      Type get_type() const;

      /**
       * Returns the stored Fragment.
       * @pre get_type() == Type::FRAGMENT
       */
      const Fragment& as_fragment() const;

      /**
       * Returns the stored children.
       * @pre get_type() == Type::CHUNK
       */
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
