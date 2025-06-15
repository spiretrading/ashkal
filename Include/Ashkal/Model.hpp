#ifndef ASHKAL_MODEL_HPP
#define ASHKAL_MODEL_HPP
#include <unordered_map>
#include "Ashkal/BoundingBox.hpp"
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Mesh.hpp"

namespace Ashkal {

  /** Encapsulates a mesh and its spatial transformation. */
  class Model {
    private:
      struct Node;
    public:

      /**
       * Represents a spatial segment within a model, corresponding to a single
       * MeshNode. Each Segment stores transformed data for the associated mesh
       * node and supports hierarchical transformation updates.
       */
      class Segment {
        public:

          /**
           * Returns the local-to-world transformation matrix of this segment.
           */
          const Matrix& get_transformation() const;

          /**
           * Returns the axis-aligned bounding box of this segment in local
           * space.
           */
          const BoundingBox& get_bounding_box() const;

          /**
           * Applies a transformation to this segment.
           * @param transformation The transformation matrix to apply.
           */
          void apply(const Matrix& transformation);

        private:
          friend struct Model::Node;
          Matrix m_transformation;
          BoundingBox m_bounding_box;

          Segment();
          Segment(const Segment&) = default;
          Segment& operator =(const Segment&) = default;
      };

      /**
       * Constructs a Model from a mesh, initializing its transformation to
       * identity.
       * @param mesh The Mesh to be rendered by this model.
       */
      explicit Model(Mesh mesh);

      /** Returns the mesh associated with this model. */
      const Mesh& get_mesh() const;

      /**
       * Returns the segment associated with one of this model's mesh nodes.
       * @param node A MeshNode belonging to this model.
       * @return The segment associated with the given node.
       */
      const Segment& get_segment(const MeshNode& node) const;

      /**
       * Returns the segment associated with one of this model's mesh nodes.
       * @param node A MeshNode belonging to this model.
       * @return The segment associated with the given node.
       */
      Segment& get_segment(const MeshNode& node);

    private:
      struct Node {
        Segment m_segment;
        std::vector<Node> m_children;

        Node(const Mesh& mesh, const MeshNode& node,
          std::unordered_map<const MeshNode*, Node*>& mesh_to_node);
      };
      Mesh m_mesh;
      std::unordered_map<const MeshNode*, Node*> m_mesh_to_node;
      Node m_root;

      Model(const MeshNode& node);
  };

  /**
   * Returns the bounding box that encloses all vertices used by a given
   * fragment.
   * @param mesh The mesh that contains the vertex data.
   * @param fragment The fragment whose bounding box is to be computed.
   * @return A BoundingBox that tightly contains all triangle vertices in the
   *         fragment.
   */
  inline BoundingBox make_bounding_box(
      const Mesh& mesh, const Fragment& fragment) {
    auto& vertices = mesh.m_vertices;
    auto& triangles = fragment.get_triangles();
    if(triangles.empty()) {
      return BoundingBox(Point(0, 0, 0), Point(0, 0, 0));
    }
    auto& first_vertex = vertices[triangles[0].m_a].m_position;
    auto min_x = first_vertex.m_x;
    auto min_y = first_vertex.m_y;
    auto min_z = first_vertex.m_z;
    auto max_x = first_vertex.m_x;
    auto max_y = first_vertex.m_y;
    auto max_z = first_vertex.m_z;
    for(auto& triangle : triangles) {
      for(auto index : {triangle.m_a, triangle.m_b, triangle.m_c}) {
        auto& position = vertices[index].m_position;
        min_x = std::min(min_x, position.m_x);
        min_y = std::min(min_y, position.m_y);
        min_z = std::min(min_z, position.m_z);
        max_x = std::max(max_x, position.m_x);
        max_y = std::max(max_y, position.m_y);
        max_z = std::max(max_z, position.m_z);
      }
    }
    return BoundingBox(Point(min_x, min_y, min_z), Point(max_x, max_y, max_z));
  }

  /**
   * Returns the bounding box of a MeshNode.
   * @param mesh The mesh that contains the vertex data.
   * @param node The mesh node to compute a bounding box for.
   * @return A BoundingBox that tightly encloses the geometry in the node.
   */
  inline BoundingBox make_bounding_box(const Mesh& mesh, const MeshNode& node) {
    if(node.get_type() == MeshNode::Type::FRAGMENT) {
      return make_bounding_box(mesh, node.as_fragment());
    }
    auto& children = node.as_chunk();
    if(children.empty()) {
      return BoundingBox(Point(0, 0, 0), Point(0, 0, 0));
    }
    auto combined_box = make_bounding_box(mesh, children[0]);
    for(auto i = std::size_t(1); i < children.size(); ++i) {
      combined_box = merge(combined_box, make_bounding_box(mesh, children[i]));
    }
    return combined_box;
  }

  inline Model::Model(Mesh mesh)
    : m_mesh(std::move(mesh)),
      m_root(m_mesh, m_mesh.m_root, m_mesh_to_node) {}

  inline const Mesh& Model::get_mesh() const {
    return m_mesh;
  }

  inline const Model::Segment& Model::get_segment(const MeshNode& node) const {
    return m_mesh_to_node.at(&node)->m_segment;
  }

  inline Model::Segment& Model::get_segment(const MeshNode& node) {
    return m_mesh_to_node.at(&node)->m_segment;
  }

  inline Model::Node::Node(const Mesh& mesh, const MeshNode& node,
      std::unordered_map<const MeshNode*, Node*>& mesh_to_node) {
    m_segment.m_bounding_box = make_bounding_box(mesh, node);
    mesh_to_node.insert(std::pair(&node, this));
    if(node.get_type() == MeshNode::Type::CHUNK) {
      m_children.reserve(node.as_chunk().size());
      for(auto& child : node.as_chunk()) {
        m_children.emplace_back(mesh, child, mesh_to_node);
      }
    }
  }

  inline const Matrix& Model::Segment::get_transformation() const {
    return m_transformation;
  }

  inline const BoundingBox& Model::Segment::get_bounding_box() const {
    return m_bounding_box;
  }

  inline void Model::Segment::apply(const Matrix& transformation) {
    m_transformation = transformation * m_transformation;
  }

  inline Model::Segment::Segment()
    : m_transformation(Matrix::IDENTITY()) {}
}

#endif
