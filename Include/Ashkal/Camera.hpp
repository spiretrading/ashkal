#ifndef ASHKAL_CAMERA_HPP
#define ASHKAL_CAMERA_HPP
#include <ostream>
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Represents a camera within a voxel space. */
  class Camera {
    public:

      /** The z-position of the near plane. */
      static inline const auto NEAR_PLANE_Z = -1.f;

      /**
       * Constructs a Camera at the origin (0, 0, 0), facing forward (0, 0, 1),
       * and oriented upwards (0, 1, 0).
       */
      Camera();

      /**
       * Constructs a Camera at a given position, with a given direction and
       * orientation.
       */
      Camera(Point position, Vector direction, Vector orientation);

      /** Returns the view to world matrix. */
      const Matrix& get_view_to_world() const;

      /** Returns the camera's position. */
      Point get_position() const;

      /** Returns the camera's direction. */
      Vector get_direction() const;

      /** Returns the camera's orientation. */
      Vector get_orientation() const;

      /** Returns the camera's rightward direction. */
      Vector get_right() const;

      /** Applies a transformation to this camera. */
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

  /** Moves a camera forward by a given distance. */
  inline void move_forward(Camera& camera, float distance) {
    camera.apply(translate(distance * camera.get_direction()));
  }

  /** Moves a camera backward by a given distance. */
  inline void move_backward(Camera& camera, float distance) {
    move_forward(camera, -distance);
  }

  /** Moves a camera left by a given distance. */
  inline void move_left(Camera& camera, float distance) {
    auto roll = cross(camera.get_orientation(), camera.get_direction());
    camera.apply(translate(distance * -roll));
  }

  /** Moves a camera right by a given distance. */
  inline void move_right(Camera& camera, float distance) {
    move_left(camera, -distance);
  }

  /** Moves a camera up by a given distance. */
  inline void move_up(Camera& camera, float distance) {
    camera.apply(translate(distance * camera.get_orientation()));
  }

  /** Moves a camera down by a given distance. */
  inline void move_down(Camera& camera, float distance) {
    move_up(camera, -distance);
  }

  /** Tilts a camera horizontally and vertically. */
  inline void tilt(Camera& camera, float tilt_x, float tilt_y) {
    camera.apply(translate(Vector(camera.get_position())) * yaw(tilt_x) *
      pitch(-tilt_y) * translate(-Vector(camera.get_position())));
  }

  /** Rolls the camera. */
  inline void roll(Camera& camera, float radians) {
    camera.apply(rotate(camera.get_direction(), radians));
  }

  /** Tests if a point is in front of the camera. */
  inline bool is_in_front(const Point& point) {
    return point.m_z < -Camera::NEAR_PLANE_Z;
  }

  /** Transforms a point from the world space to camera space. */
  inline Point world_to_view(const Point& point, const Camera& camera) {
    auto delta = point - camera.get_position();
    return Point(dot(delta, camera.get_right()),
      dot(delta, camera.get_orientation()),
      dot(delta, -camera.get_direction()));
  }

  inline std::ostream& operator<<(std::ostream& out, const Camera& camera) {
    return out << "Camera(" << camera.get_position() << ", " <<
      camera.get_direction() << ", " << camera.get_orientation() << ", " <<
      camera.get_right() << ')';
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
