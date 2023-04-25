#ifndef ASHKAL_CUBE_HPP
#define ASHKAL_CUBE_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Model.hpp"

namespace Ashkal {

  /** Models a cube. */
  class Cube : public Model {
    public:

      /** Constructs a Cube with a given side length and color. */
      Cube(int size, Color color);

      Point end() const override;

      Voxel get(Point point) const override;

    private:
      int m_size;
      Color m_color;
  };

  inline Cube::Cube(int size, Color color)
    : m_size(size),
      m_color(color) {}

  inline Point Cube::end() const {
    return Point(static_cast<float>(m_size), static_cast<float>(m_size),
      static_cast<float>(m_size));
  }

  inline Voxel Cube::get(Point point) const {
    if(point.m_x >= 0 && point.m_y >= 0 && point.m_z >= 0 &&
        point.m_x < m_size && point.m_y < m_size && point.m_z < m_size) {
      return Voxel(m_color);
    }
    return Voxel::NONE();
  }
}

#endif
