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

      const Matrix& get_world_to_view() const;

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
      Matrix m_world_to_view;
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
    auto& world_to_view = camera.get_world_to_view();
    auto view_to_world = invert(world_to_view);
    camera.apply(yaw(tilt_x) * pitch(tilt_y) * view_to_world);
    camera.apply(world_to_view);
  }

  inline Camera::Camera()
    : m_world_to_view(Matrix::IDENTITY()) {}

  inline Camera::Camera(Point position, Vector direction, Vector orientation) {
    m_world_to_view.set(POSITION_COLUMN, X_INDEX, position.m_x);
    m_world_to_view.set(POSITION_COLUMN, Y_INDEX, position.m_y);
    m_world_to_view.set(POSITION_COLUMN, Z_INDEX, position.m_z);
    m_world_to_view.set(POSITION_COLUMN, H_INDEX, 1);
    m_world_to_view.set(X_INDEX, DIRECTION_ROW, direction.m_x);
    m_world_to_view.set(Y_INDEX, DIRECTION_ROW, direction.m_y);
    m_world_to_view.set(Z_INDEX, DIRECTION_ROW, direction.m_z);
    m_world_to_view.set(X_INDEX, ORIENTATION_ROW, orientation.m_x);
    m_world_to_view.set(Y_INDEX, ORIENTATION_ROW, orientation.m_y);
    m_world_to_view.set(Z_INDEX, ORIENTATION_ROW, orientation.m_z);
    auto right = cross(orientation, direction);
    m_world_to_view.set(X_INDEX, RIGHT_ROW, right.m_x);
    m_world_to_view.set(Y_INDEX, RIGHT_ROW, right.m_y);
    m_world_to_view.set(Z_INDEX, RIGHT_ROW, right.m_z);
    m_world_to_view.set(X_INDEX, HOMOGENEOUS_ROW, 0);
    m_world_to_view.set(Y_INDEX, HOMOGENEOUS_ROW, 0);
    m_world_to_view.set(Z_INDEX, HOMOGENEOUS_ROW, 0);
    m_world_to_view.set(H_INDEX, HOMOGENEOUS_ROW, 1);
  }

  inline const Matrix& Camera::get_world_to_view() const {
    return m_world_to_view;
  }

  inline Point Camera::get_position() const {
    return Point(m_world_to_view.get(POSITION_COLUMN, X_INDEX),
      m_world_to_view.get(POSITION_COLUMN, Y_INDEX),
      m_world_to_view.get(POSITION_COLUMN, Z_INDEX));
  }

  inline Vector Camera::get_direction() const {
    return Vector(m_world_to_view.get(X_INDEX, DIRECTION_ROW),
      m_world_to_view.get(Y_INDEX, DIRECTION_ROW),
      m_world_to_view.get(Z_INDEX, DIRECTION_ROW));
  }

  inline Vector Camera::get_orientation() const {
    return Vector(m_world_to_view.get(X_INDEX, ORIENTATION_ROW),
      m_world_to_view.get(Y_INDEX, ORIENTATION_ROW),
      m_world_to_view.get(Z_INDEX, ORIENTATION_ROW));
  }

  inline Vector Camera::get_right() const {
    return Vector(m_world_to_view.get(X_INDEX, RIGHT_ROW),
      m_world_to_view.get(Y_INDEX, RIGHT_ROW),
      m_world_to_view.get(Z_INDEX, RIGHT_ROW));
  }

  inline void Camera::apply(const Matrix& transformation) {
    m_world_to_view = transformation * m_world_to_view;
//    m_view_to_world = invert(transformation) * m_view_to_world;
  }
}

#endif
