#ifndef ASHKAL_FRAGMENT_HPP
#define ASHKAL_FRAGMENT_HPP
#include <memory>
#include <vector>
#include "Ashkal/Material.hpp"
#include "Ashkal/VertexTriangle.hpp"

namespace Ashkal {

  /**
   * Represents a contiguous piece of geometry (made up of VertexTriangle
   * instances) together with the Material used to shade it.
   */
  class Fragment {
    public:

      /**
       * Constructs a fragment from a set of triangles and a material.
       * @param triangles The triangles composing this fragment.
       * @param material The material to apply.
       */
      Fragment(std::vector<VertexTriangle> triangles,
        std::shared_ptr<Material> material);

      /** Returns the triangles in this fragment. */
      const std::vector<VertexTriangle>& get_triangles() const;

      /** Returns the material of this fragment. */
      const Material& get_material() const;

    private:
      std::vector<VertexTriangle> m_triangles;
      std::shared_ptr<Material> m_material;
  };

  inline Fragment::Fragment(
    std::vector<VertexTriangle> triangles, std::shared_ptr<Material> material)
    : m_triangles(std::move(triangles)),
      m_material(std::move(material)) {}

  inline const std::vector<VertexTriangle>& Fragment::get_triangles() const {
    return m_triangles;
  }

  inline const Material& Fragment::get_material() const {
    return *m_material;
  }
}

#endif
