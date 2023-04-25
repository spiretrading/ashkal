#ifndef ASHKAL_SPHERE_HPP
#define ASHKAL_SPHERE_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Model.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Models a sphere. */
  class Sphere : public Model {
    public:

      /** Constructs a Sphere with a given radius and color. */
      Sphere(int radius, Color color);

      Point end() const override;

      Voxel get(Point point) const override;

    private:
      int m_radius;
      Point m_inner;
      Color m_color;
  };

  inline Sphere::Sphere(int radius, Color color)
    : m_radius(radius),
      m_inner(float(m_radius - 1), float(m_radius - 1), float(m_radius - 1)),
      m_color(color) {}

  inline Point Sphere::end() const {
    return Point(2 * static_cast<float>(m_radius) - 1,
      2 * static_cast<float>(m_radius) - 1,
      2 * static_cast<float>(m_radius) - 1);
  }

  inline Voxel Sphere::get(Point point) const {
    auto range = floor(point) - m_inner;
    if(dot(range, range) <= m_radius * m_radius) {
      return Voxel(m_color);
    }
    return Voxel::NONE();
  }
}

#endif
