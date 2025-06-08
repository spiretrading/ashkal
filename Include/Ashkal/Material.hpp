#ifndef ASHKAL_MATERIAL_HPP
#define ASHKAL_MATERIAL_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/ColorSampler.hpp"

namespace Ashkal {
  class Material {
    public:
      explicit Material(std::shared_ptr<ColorSampler> diffuseness);

      const ColorSampler& get_diffuseness() const;

    private:
      std::shared_ptr<ColorSampler> m_diffuseness;
  };

  inline Material::Material(std::shared_ptr<ColorSampler> diffuseness)
    : m_diffuseness(std::move(diffuseness)) {}

  inline const ColorSampler& Material::get_diffuseness() const {
    return *m_diffuseness;
  }
}

#endif
