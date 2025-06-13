#ifndef ASHKAL_TRANSFORMATION_HPP
#define ASHKAL_TRANSFORMATION_HPP
#include <unordered_map>
#include <vector>
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Mesh.hpp"

namespace Ashkal {

  /**
   * Builds and maintains a mapping from each MeshNode to a transformation
   * matrix, allowing individual nodes (and their subtrees) to be transformed
   * relative to their parent.
   */
  class Transformation {
    public:

      /**
       * Constructs the transformation context for a mesh.
       * @param mesh The Mesh whose node hierarchy to track.
       */
      explicit Transformation(const Mesh& mesh);

      /**
       * Returns the current world-space transformation for a node.
       * @param node The MeshNode whose matrix is requested.
       * @return The transformation matrix for that node.
       */
      const Matrix& get_transformation(const MeshNode& node) const;

      /**
       * Applies an additional transformation to a node and its subtree.
       * @param transformation The matrix to prepend.
       * @param node The MeshNode to transform.
       */
      void apply(const Matrix& transformation, const MeshNode& node);

    private:
      struct Node {
        Matrix m_transformation;
        std::vector<Node> m_children;

        Node(const MeshNode& node,
          std::unordered_map<const MeshNode*, Node*>& mesh_to_node);
      };
      std::unordered_map<const MeshNode*, Node*> m_mesh_to_node;
      Node m_root;

      Transformation(const Transformation&) = delete;
      Transformation& operator =(const Transformation&) = delete;
      Transformation(const MeshNode& node);
  };

  inline Transformation::Transformation(const Mesh& mesh)
    : m_root(mesh.m_root, m_mesh_to_node) {}

  inline const Matrix& Transformation::get_transformation(
      const MeshNode& node) const {
    return m_mesh_to_node.at(&node)->m_transformation;
  }

  inline void Transformation::apply(
      const Matrix& transformation, const MeshNode& node) {
    auto& child = m_mesh_to_node.at(&node);
    child->m_transformation = transformation * child->m_transformation;
  }

  inline Transformation::Node::Node(const MeshNode& node,
      std::unordered_map<const MeshNode*, Node*>& mesh_to_node)
      : m_transformation(Matrix::IDENTITY()) {
    mesh_to_node.insert(std::pair(&node, this));
    if(node.get_type() == MeshNode::Type::CHUNK) {
      m_children.reserve(node.as_chunk().size());
      for(auto& child : node.as_chunk()) {
        m_children.emplace_back(child, mesh_to_node);
      }
    }
  }
}

#endif
