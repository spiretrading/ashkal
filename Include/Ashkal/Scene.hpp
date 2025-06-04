#ifndef ASHKAL_SCENE_HPP
#define ASHKAL_SCENE_HPP
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/SceneElement.hpp"

namespace Ashkal {
  class Scene {
    public:
      Scene();

      int get_scene_element_count() const;

      SceneElement& get_scene_element(int index);

      const SceneElement& get_scene_element(int index) const;

      void add(std::unique_ptr<SceneElement> element);

      void remove_scene_element(int index);

      const AmbientLight& get_ambient_light() const;

      void set(const AmbientLight& light);

    private:
      std::vector<std::unique_ptr<SceneElement>> m_elements;
      AmbientLight m_ambient_light;

      Scene(const Scene&) = delete;
      Scene& operator =(const Scene&) = delete;
  };

  inline Scene::Scene()
    : m_ambient_light(Color(0, 0, 0, 255)) {}

  inline int Scene::get_scene_element_count() const {
    return static_cast<int>(m_elements.size());
  }

  inline SceneElement& Scene::get_scene_element(int index) {
    return *m_elements[index];
  }

  inline const SceneElement& Scene::get_scene_element(int index) const {
    return *m_elements[index];
  }

  inline void Scene::add(std::unique_ptr<SceneElement> element) {
    m_elements.push_back(std::move(element));
  }

  inline void Scene::remove_scene_element(int index) {
    if(index == m_elements.size() - 1) {
      m_elements.pop_back();
    }
    std::swap(m_elements[index], m_elements.back());
    m_elements.pop_back();
  }

  inline const AmbientLight& Scene::get_ambient_light() const {
    return m_ambient_light;
  }

  inline void Scene::set(const AmbientLight& light) {
    m_ambient_light = light;
  }
}

#endif
