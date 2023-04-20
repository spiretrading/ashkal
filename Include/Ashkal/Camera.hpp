#ifndef ASHKAL_CAMERA_HPP
#define ASHKAL_CAMERA_HPP
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  class Camera {
    public:
      Point get_position() const;

      void set_position(Point position);

      Vector get_direction() const;

      void set_direction(Vector direction);

      Vector get_orientation() const;

      void set_orientation(Vector orientation);

    private:
      Point m_position;
      Vector m_direction;
      Vector m_orientation;
  };

  inline void move_forward(Camera& camera, float distance) {
    camera.set_position(
      translate(distance * camera.get_direction()) * camera.get_position());
  }

  inline void move_backward(Camera& camera, float distance) {
    camera.set_position(
      translate(distance * -camera.get_direction()) * camera.get_position());
  }

  inline void move_left(Camera& camera, float distance) {
    auto roll = cross(camera.get_orientation(), camera.get_direction());
    camera.set_position(translate(distance * -roll) * camera.get_position());
  }

  inline void move_right(Camera& camera, float distance) {
    auto roll = cross(camera.get_orientation(), camera.get_direction());
    camera.set_position(translate(distance * roll) * camera.get_position());
  }

  inline void tilt(Camera& camera, float tilt_x, float tilt_y) {
    auto rotation_x = yaw(tilt_x);
    auto rotation_y = pitch(tilt_y);
  }

  inline Point Camera::get_position() const {
    return m_position;
  }

  inline void Camera::set_position(Point position) {
    m_position = position;
  }

  inline Vector Camera::get_direction() const {
    return m_direction;
  }

  inline void Camera::set_direction(Vector direction) {
    m_direction = direction / magnitude(direction);
  }

  inline Vector Camera::get_orientation() const {
    return m_orientation;
  }

  inline void Camera::set_orientation(Vector orientation) {
    m_orientation = orientation / magnitude(orientation);
  }
}

#endif
