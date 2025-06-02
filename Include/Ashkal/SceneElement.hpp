#ifndef ASHKAL_SCENE_ELEMENT_HPP
#define ASHKAL_SCENE_ELEMENT_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Mesh.hpp"
#include "Ashkal/Transformation.hpp"

namespace Ashkal {
  class SceneElement {
    public:
      SceneElement(Mesh mesh);

      const Mesh& get_mesh() const;

      Transformation& get_transformation();

      const Transformation& get_transformation() const;

    private:
      Mesh m_mesh;
      Transformation m_transformation;
  };

  inline SceneElement::SceneElement(Mesh mesh)
    : m_mesh(std::move(mesh)),
      m_transformation(m_mesh) {}

  inline const Mesh& SceneElement::get_mesh() const {
    return m_mesh;
  }

  inline Transformation& SceneElement::get_transformation() {
    return m_transformation;
  }

  inline const Transformation& SceneElement::get_transformation() const {
    return m_transformation;
  }
}

#endif
