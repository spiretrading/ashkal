#ifndef ASHKAL_FRAGMENT_HPP
#define ASHKAL_FRAGMENT_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Material.hpp"
#include "Ashkal/VertexTriangle.hpp"

namespace Ashkal {
  class Fragment {
    public:
      Fragment(std::vector<VertexTriangle> triangles,
        std::shared_ptr<Material> material);

      const std::vector<VertexTriangle>& get_triangles() const;

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
