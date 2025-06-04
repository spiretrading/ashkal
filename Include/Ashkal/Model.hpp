#ifndef ASHKAL_MODEL_HPP
#define ASHKAL_MODEL_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Mesh.hpp"
#include "Ashkal/Transformation.hpp"

namespace Ashkal {
  class Model {
    public:
      Model(Mesh mesh);

      const Mesh& get_mesh() const;

      Transformation& get_transformation();

      const Transformation& get_transformation() const;

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

  inline Transformation& Model::get_transformation() {
    return m_transformation;
  }

  inline const Transformation& Model::get_transformation() const {
    return m_transformation;
  }
}

#endif
