#ifndef ASHKAL_OCTREE_HPP
#define ASHKAL_OCTREE_HPP
#include <memory>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Model.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Ray.hpp"
#include "Ashkal/Vector.hpp"
#include "Ashkal/Voxel.hpp"

namespace Ashkal {
  class OctreeNode {
    public:
      virtual ~OctreeNode() = default;

      Point get_start() const {
        return m_start;
      }

      Point get_end() const {
        return m_end;
      }

      virtual Voxel get(Point point) const = 0;

      virtual Voxel intersect(Point& point, Vector direction) const = 0;

      virtual void add(std::shared_ptr<Model> model) = 0;

    protected:
      OctreeNode(Point start, int size)
        : m_start(start),
        m_end(Point(start.m_x + size, start.m_y + size, start.m_z + size)) {}

    private:
      Point m_start;
      Point m_end;
  };

  class OctreeLeaf : public OctreeNode {
    public:
      OctreeLeaf(Point start, int size)
        : OctreeNode(start, size) {}

      Voxel get(Point point) const override {
        for (auto& model : m_models) {
          auto voxel = model->get(point);
          if (voxel != Voxel::NONE()) {
            return voxel;
          }
        }
        return Voxel::NONE();
      }

      Voxel intersect(Point& point, Vector direction) const override {
        if (m_models.empty()) {
          point = compute_boundary(Ray(point, direction), get_start(),
            static_cast<int>(get_end().m_x - get_start().m_x));
          return Voxel::NONE();
        }
        while (contains(get_start(), get_end(), point)) {
          auto voxel = get(point);
          if (voxel != Voxel::NONE()) {
            return voxel;
          }
          point = compute_boundary(Ray(point, direction), floor(point), 1);
        }
        return Voxel::NONE();
      }

      void add(std::shared_ptr<Model> model) override {
        m_models.push_back(std::move(model));
      }

    private:
      std::vector<std::shared_ptr<Model>> m_models;
  };

  class OctreeInternalNode : public OctreeNode {
    public:
      OctreeInternalNode(Point start, int size)
          : OctreeNode(start, size),
            m_is_empty(true) {
        if (size >= 128) {
          m_children[0] = std::make_unique<OctreeInternalNode>(start, size / 2);
          m_children[1] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x, start.m_y, start.m_z + size / 2), size / 2);
          m_children[2] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x, start.m_y + size / 2, start.m_z), size / 2);
          m_children[3] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x, start.m_y + size / 2, start.m_z + size / 2),
            size / 2);
          m_children[4] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x + size / 2, start.m_y, start.m_z), size / 2);
          m_children[5] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x + size / 2, start.m_y, start.m_z + size / 2),
            size / 2);
          m_children[6] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x + size / 2, start.m_y + size / 2, start.m_z),
            size / 2);
          m_children[7] = std::make_unique<OctreeInternalNode>(
            Point(start.m_x + size / 2, start.m_y + size / 2,
              start.m_z + size / 2), size / 2);
        } else {
          m_children[0] = std::make_unique<OctreeLeaf>(start, size / 2);
          m_children[1] = std::make_unique<OctreeLeaf>(
            Point(start.m_x, start.m_y, start.m_z + size / 2), size / 2);
          m_children[2] = std::make_unique<OctreeLeaf>(
            Point(start.m_x, start.m_y + size / 2, start.m_z), size / 2);
          m_children[3] = std::make_unique<OctreeLeaf>(
            Point(start.m_x, start.m_y + size / 2, start.m_z + size / 2),
            size / 2);
          m_children[4] = std::make_unique<OctreeLeaf>(
            Point(start.m_x + size / 2, start.m_y, start.m_z), size / 2);
          m_children[5] = std::make_unique<OctreeLeaf>(Point(
            start.m_x + size / 2, start.m_y, start.m_z + size / 2), size / 2);
          m_children[6] = std::make_unique<OctreeLeaf>(Point(
            start.m_x + size / 2, start.m_y + size / 2, start.m_z), size / 2);
          m_children[7] = std::make_unique<OctreeLeaf>(Point(
            start.m_x + size / 2, start.m_y + size / 2, start.m_z + size / 2),
            size / 2);
        }
      }

      Voxel get(Point point) const override {
        return get_node(point).get(point);
      }

      Voxel intersect(Point& point, Vector direction) const {
        if(m_is_empty) {
          point = compute_boundary(Ray(point, direction), get_start(),
            static_cast<int>(get_end().m_x - get_start().m_x));
          return Voxel::NONE();
        }
        while(contains(get_start(), get_end(), point)) {
          auto voxel = get_node(point).intersect(point, direction);
          if(voxel != Voxel::NONE()) {
            return voxel;
          }
        }
        return Voxel::NONE();
      }

      void add(std::shared_ptr<Model> model) {
        if((0 <= get_start().m_x && model->end().m_x > get_start().m_x ||
            0 < get_end().m_x && model->end().m_x > get_start().m_x) &&
            (0 <= get_start().m_y && model->end().m_y > get_start().m_y ||
              0 < get_end().m_y && model->end().m_y > get_start().m_y) &&
            (0 <= get_start().m_z && model->end().m_z > get_start().m_z ||
              0 < get_end().m_z && model->end().m_z > get_start().m_z)) {
          m_is_empty = false;
          for(auto& child : m_children) {
            child->add(model);
          }
        }
      }

    private:
      bool m_is_empty;
      std::array<std::unique_ptr<OctreeNode>, 8> m_children;

      const OctreeNode& get_node(Point point) const {
        return *m_children[get_node_index(point)];
      }

      OctreeNode& get_node(Point point) {
        return *m_children[get_node_index(point)];
      }

      int get_node_index(Point point) const {
        auto size = get_end().m_x - get_start().m_x;
        if(point.m_x < get_start().m_x + size / 2) {
          if(point.m_y < get_start().m_y + size / 2) {
            if(point.m_z < get_start().m_z + size / 2) {
              return 0;
            }
            return 1;
          }
          if(point.m_z < get_start().m_z + size / 2) {
            return 2;
          }
          return 3;
        }
        if(point.m_y < get_start().m_y + size / 2) {
          if(point.m_z < get_start().m_z + size / 2) {
            return 4;
          }
          return 5;
        }
        if(point.m_z < get_start().m_z + size / 2) {
          return 6;
        } else {
          return 7;
        }
      }
  };
}

#endif
