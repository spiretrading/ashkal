#ifndef ASHKAL_SCENE_HPP
#define ASHKAL_SCENE_HPP
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/DirectionalLight.hpp"
#include "Ashkal/Model.hpp"

namespace Ashkal {
  class Scene {
    public:
      Scene();

      int get_model_count() const;

      Model& get_model(int index);

      const Model& get_model(int index) const;

      void add(std::unique_ptr<Model> model);

      void remove_model(int index);

      const AmbientLight& get_ambient_light() const;

      void set(const AmbientLight& light);

      const DirectionalLight& get_directional_light() const;

      void set(const DirectionalLight& light);

    private:
      std::vector<std::unique_ptr<Model>> m_models;
      AmbientLight m_ambient_light;
      DirectionalLight m_directional_light;

      Scene(const Scene&) = delete;
      Scene& operator =(const Scene&) = delete;
  };

  inline Scene::Scene()
    : m_ambient_light(Color(0, 0, 0, 255)) {}

  inline int Scene::get_model_count() const {
    return static_cast<int>(m_models.size());
  }

  inline Model& Scene::get_model(int index) {
    return *m_models[index];
  }

  inline const Model& Scene::get_model(int index) const {
    return *m_models[index];
  }

  inline void Scene::add(std::unique_ptr<Model> model) {
    m_models.push_back(std::move(model));
  }

  inline void Scene::remove_model(int index) {
    if(index == m_models.size() - 1) {
      m_models.pop_back();
    }
    std::swap(m_models[index], m_models.back());
    m_models.pop_back();
  }

  inline const AmbientLight& Scene::get_ambient_light() const {
    return m_ambient_light;
  }

  inline void Scene::set(const AmbientLight& light) {
    m_ambient_light = light;
  }

  inline const DirectionalLight& Scene::get_directional_light() const {
    return m_directional_light;
  }

  inline void Scene::set(const DirectionalLight& light) {
    m_directional_light = light;
  }
}

#endif
