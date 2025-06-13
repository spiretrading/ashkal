#ifndef ASHKAL_MATERIAL_HPP
#define ASHKAL_MATERIAL_HPP
#include <memory>
#include "Ashkal/ColorSampler.hpp"

namespace Ashkal {

  /**
   * Encapsulates a ColorSampler to provide diffuse colors for shading
   * operations.
   */
  class Material {
    public:

      /**
       * Constructs a Material with a diffuseness sampler.
       * @param  diffuseness The ColorSampler providing diffuse color lookups.
       */
      explicit Material(std::shared_ptr<ColorSampler> diffuseness);

      /** Returns the material's diffuseness sampler. */
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
