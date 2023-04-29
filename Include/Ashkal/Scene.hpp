#ifndef ASHKAL_SCENE_HPP
#define ASHKAL_SCENE_HPP
#include <memory>
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/DirectionalLight.hpp"
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Model.hpp"
#include "Ashkal/Octree.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  class Scene {
    public:
      struct IntersectionResult {
        Voxel m_voxel;
        Point m_position;
      };

      Scene()
        : m_root(Point(-2048, -2048, -2048), 4096) {}

      Voxel get(Point point) const {
        return m_root.get(point);
      }

      IntersectionResult intersect(Point point, Vector direction) const {
        auto intersection = IntersectionResult();
        intersection.m_voxel = m_root.intersect(point, direction);
        intersection.m_position = point;
        return intersection;
      }

      AmbientLight get_ambient_light() const {
        return m_ambient_light;
      }

      void set(AmbientLight light) {
        m_ambient_light = light;
      }

      DirectionalLight get_directional_light() const {
        return m_directional_light;
      }

      void set(DirectionalLight light) {
        m_directional_light = light;
      };

      void add(std::shared_ptr<Model> model) {
        m_root.add(model);
      }

    private:
      AmbientLight m_ambient_light;
      DirectionalLight m_directional_light;
      OctreeInternalNode m_root;
  };
}

#endif
