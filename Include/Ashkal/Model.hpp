#ifndef ASHKAL_MODEL_HPP
#define ASHKAL_MODEL_HPP
#include "Ashkal/Mesh.hpp"
#include "Ashkal/Transformation.hpp"

namespace Ashkal {

  /** Encapsulates a mesh and its spatial transformation. */
  class Model {
    public:

      /**
       * Constructs a Model from a mesh, initializing its transformation to
       * identity.
       * @param mesh The Mesh to be rendered by this model.
       */
      explicit Model(Mesh mesh);

      /** Returns the mesh associated with this model. */
      const Mesh& get_mesh() const;

      /** Returns this model's transformation. */
      const Transformation& get_transformation() const;

      /** Returns this model's transformation. */
      Transformation& get_transformation();

    private:
      Mesh m_mesh;
      Transformation m_transformation;
  };

  inline Model::Model(Mesh mesh)
    : m_mesh(std::move(mesh)),
      m_transformation(m_mesh) {}

  inline const Mesh& Model::get_mesh() const {
    return m_mesh;
  }

  inline const Transformation& Model::get_transformation() const {
    return m_transformation;
  }

  inline Transformation& Model::get_transformation() {
    return m_transformation;
  }
}

#endif
