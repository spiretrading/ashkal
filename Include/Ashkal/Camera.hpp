#ifndef ASHKAL_CAMERA_HPP
#define ASHKAL_CAMERA_HPP
#include <iostream>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {
  class Camera {
    public:
      Camera();

      Camera(Point position, Vector direction, Vector orientation);

      const Matrix& get_view_to_world() const;

      Point get_position() const;

      Vector get_direction() const;

      Vector get_orientation() const;

      Vector get_right() const;

      void apply(const Matrix& transformation);

    private:
      static const auto RIGHT_ROW = 0;
      static const auto ORIENTATION_ROW = 1;
      static const auto DIRECTION_ROW = 2;
      static const auto HOMOGENEOUS_ROW = 3;
      static const auto POSITION_COLUMN = 3;
      static const auto X_INDEX = 0;
      static const auto Y_INDEX = 1;
      static const auto Z_INDEX = 2;
      static const auto H_INDEX = 3;
      Matrix m_view_to_world;
  };

  inline void move_forward(Camera& camera, float distance) {
    camera.apply(translate(distance * camera.get_direction()));
  }

  inline void move_backward(Camera& camera, float distance) {
    move_forward(camera, -distance);
  }

  inline void move_left(Camera& camera, float distance) {
    auto roll = cross(camera.get_orientation(), camera.get_direction());
    camera.apply(translate(distance * -roll));
  }

  inline void move_right(Camera& camera, float distance) {
    move_left(camera, -distance);
  }

  inline void tilt(Camera& camera, float tilt_x, float tilt_y) {
    auto view_to_world = camera.get_view_to_world();
    auto world_to_view = invert(view_to_world);
    std::cout << "Original:" << std::endl;
    std::cout << view_to_world << std::endl;
    std::cout << "Local transform: " << std::endl;
    auto local = yaw(tilt_x) * pitch(tilt_y);
    std::cout << local << std::endl;
    std::cout << "Global transform: " << std::endl;
    std::cout << view_to_world * local << std::endl;
/*
    camera.apply(yaw(tilt_x) * pitch(tilt_y) * world_to_view);
    std::cout << camera.get_view_to_world() << std::endl;
    camera.apply(view_to_world);
    std::cout << camera.get_view_to_world() << std::endl;
*/
  }

  inline Camera::Camera()
    : m_view_to_world(Matrix::IDENTITY()) {}

  inline Camera::Camera(Point position, Vector direction, Vector orientation) {
    m_view_to_world.set(POSITION_COLUMN, X_INDEX, position.m_x);
    m_view_to_world.set(POSITION_COLUMN, Y_INDEX, position.m_y);
    m_view_to_world.set(POSITION_COLUMN, Z_INDEX, position.m_z);
    m_view_to_world.set(POSITION_COLUMN, H_INDEX, 1);
    m_view_to_world.set(X_INDEX, DIRECTION_ROW, direction.m_x);
    m_view_to_world.set(Y_INDEX, DIRECTION_ROW, direction.m_y);
    m_view_to_world.set(Z_INDEX, DIRECTION_ROW, direction.m_z);
    m_view_to_world.set(X_INDEX, ORIENTATION_ROW, orientation.m_x);
    m_view_to_world.set(Y_INDEX, ORIENTATION_ROW, orientation.m_y);
    m_view_to_world.set(Z_INDEX, ORIENTATION_ROW, orientation.m_z);
    auto right = cross(orientation, direction);
    m_view_to_world.set(X_INDEX, RIGHT_ROW, right.m_x);
    m_view_to_world.set(Y_INDEX, RIGHT_ROW, right.m_y);
    m_view_to_world.set(Z_INDEX, RIGHT_ROW, right.m_z);
    m_view_to_world.set(X_INDEX, HOMOGENEOUS_ROW, 0);
    m_view_to_world.set(Y_INDEX, HOMOGENEOUS_ROW, 0);
    m_view_to_world.set(Z_INDEX, HOMOGENEOUS_ROW, 0);
    m_view_to_world.set(H_INDEX, HOMOGENEOUS_ROW, 1);
  }

  inline const Matrix& Camera::get_view_to_world() const {
    return m_view_to_world;
  }

  inline Point Camera::get_position() const {
    return Point(m_view_to_world.get(POSITION_COLUMN, X_INDEX),
      m_view_to_world.get(POSITION_COLUMN, Y_INDEX),
      m_view_to_world.get(POSITION_COLUMN, Z_INDEX));
  }

  inline Vector Camera::get_direction() const {
    return Vector(m_view_to_world.get(X_INDEX, DIRECTION_ROW),
      m_view_to_world.get(Y_INDEX, DIRECTION_ROW),
      m_view_to_world.get(Z_INDEX, DIRECTION_ROW));
  }

  inline Vector Camera::get_orientation() const {
    return Vector(m_view_to_world.get(X_INDEX, ORIENTATION_ROW),
      m_view_to_world.get(Y_INDEX, ORIENTATION_ROW),
      m_view_to_world.get(Z_INDEX, ORIENTATION_ROW));
  }

  inline Vector Camera::get_right() const {
    return Vector(m_view_to_world.get(X_INDEX, RIGHT_ROW),
      m_view_to_world.get(Y_INDEX, RIGHT_ROW),
      m_view_to_world.get(Z_INDEX, RIGHT_ROW));
  }

  inline void Camera::apply(const Matrix& transformation) {
    m_view_to_world = transformation * m_view_to_world;
  }
}

#endif
