#ifndef ASHKAL_CAMERA_HPP
#define ASHKAL_CAMERA_HPP
#include <cmath>
#include <numbers>
#include <ostream>
#include "Ashkal/Frustum.hpp"
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Vector.hpp"

namespace Ashkal {

  /** Represents a camera within a voxel space. */
  class Camera {
    public:

      /**
       * Constructs a Camera at the origin (0, 0, 0), facing forward (0, 0, 1),
       * and oriented upwards (0, 1, 0) with a field of view of 90 degrees.
       * The near plane is set to -1 and the far plane is at 100000.
       * @param aspect_ratio The screen's aspect ratio.
       */
      explicit Camera(float aspect_ratio);

      /**
       * Constructs a Camera at the origin (0, 0, 0), facing forward (0, 0, 1),
       * and oriented upwards (0, 1, 0) with a field of view of 90 degrees.
       * @param aspect_ratio The screen's aspect ratio.
       * @param near_plane The distance to the near plane.
       * @param far_plane The distance to the far plane.
       */
      explicit Camera(float near_plane, float far_plane, float aspect_ratio);

      /**
       * Constructs a Camera at a given position, with a given direction and
       * orientation, aspect ratio and a 90 degree field of view.
       */
      Camera(Point position, Vector direction, Vector orientation,
        float aspect_ratio);

      /**
       * Constructs a Camera at a given position, with a given direction and
       * orientation, near and far planes, aspect ratio and a 90 degree field of
       * view.
       */
      Camera(Point position, Vector direction, Vector orientation,
        float near_plane, float far_plane, float aspect_ratio);

      /**
       * Constructs a Camera at a given position, with a given direction and
       * orientation, near and far planes, aspect ratio and field of view.
       */
      Camera(Point position, Vector direction, Vector orientation,
        float aspect_ratio, float field_of_view);

      /**
       * Constructs a Camera at a given position, with a given direction and
       * orientation, near and far planes, aspect ratio and field of view.
       */
      Camera(Point position, Vector direction, Vector orientation,
        float near_plane, float far_plane, float aspect_ratio,
        float field_of_view);

      /** Returns the aspect's ratio. */
      float get_aspect_ratio() const;

      /** Returns the distance from this camera to the near clipping plane. */
      float get_near_plane() const;

      /** Returns the distance from this camera to the far clipping plane. */
      float get_far_plane() const;

      /** Returns the field of view (vertical). */
      float get_field_of_view() const;

      /** Returns the focal length. */
      float get_focal_length() const;

      /** Returns the horizontal focal length. */
      float get_horizontal_focal_length() const;

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

      /** Returns the camera's frustum. */
      const Frustum& get_frustum() const;

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
      float m_aspect_ratio;
      float m_near_plane;
      float m_far_plane;
      float m_field_of_view;
      float m_focal_length;
      float m_horizontal_focal_length;
      Matrix m_view_to_world;
      Frustum m_frustum;
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
  inline bool is_in_front(const Camera& camera, const Point& point) {
    return point.m_z <= camera.get_near_plane();
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

  inline Camera::Camera(float aspect_ratio)
    : Camera(-1, -100000, aspect_ratio) {}

  inline Camera::Camera(float near_plane, float far_plane, float aspect_ratio)
    : Camera(Point(0, 0, 0), Vector(0, 0, 1), Vector(0, 1, 0), near_plane,
        far_plane, aspect_ratio) {}

  inline Camera::Camera(
    Point position, Vector direction, Vector orientation, float aspect_ratio)
    : Camera(position, direction, orientation, -1, -100000, aspect_ratio) {}

  inline Camera::Camera(Point position, Vector direction, Vector orientation,
    float near_plane, float far_plane, float aspect_ratio)
    : Camera(position, direction, orientation, near_plane, far_plane,
        aspect_ratio, std::numbers::pi / 2) {}

  inline Camera::Camera(Point position, Vector direction, Vector orientation,
    float aspect_ratio, float field_of_view)
    : Camera(position, direction, orientation, -1, -100000, aspect_ratio,
        field_of_view) {}

  inline Camera::Camera(Point position, Vector direction, Vector orientation,
      float near_plane, float far_plane, float aspect_ratio,
      float field_of_view)
      : m_near_plane(near_plane),
        m_far_plane(far_plane),
        m_aspect_ratio(aspect_ratio),
        m_field_of_view(field_of_view),
        m_focal_length(1 / std::tan(0.5f * m_field_of_view)),
        m_horizontal_focal_length(m_focal_length / m_aspect_ratio) {
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
    m_frustum.update(*this);
  }

  inline float Camera::get_aspect_ratio() const {
    return m_aspect_ratio;
  }

  inline float Camera::get_near_plane() const {
    return m_near_plane;
  }

  inline float Camera::get_far_plane() const {
    return m_far_plane;
  }

  inline float Camera::get_field_of_view() const {
    return m_field_of_view;
  }

  inline float Camera::get_focal_length() const {
    return m_focal_length;
  }

  inline float Camera::get_horizontal_focal_length() const {
    return m_horizontal_focal_length;
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

  inline const Frustum& Camera::get_frustum() const {
    return m_frustum;
  }

  inline void Camera::apply(const Matrix& transformation) {
    m_view_to_world = transformation * m_view_to_world;
    m_frustum.update(*this);
  }

  inline void Frustum::update(const Camera& camera) {
    auto tan_half_field_of_view = std::tan(camera.get_field_of_view() * 0.5f);
    auto near_plane_center =
      camera.get_position() + -camera.get_near_plane() * camera.get_direction();
    auto near_plane_half_height =
      tan_half_field_of_view * -camera.get_near_plane();
    auto near_plane_half_width =
      camera.get_aspect_ratio() * near_plane_half_height;
    auto near_plane_top_left = near_plane_center +
      near_plane_half_height * camera.get_orientation() -
      near_plane_half_width * camera.get_right();
    auto near_plane_top_right =
      near_plane_top_left + 2 * near_plane_half_width * camera.get_right();
    auto near_plane_bottom_left = near_plane_top_left -
      2 * near_plane_half_height * camera.get_orientation();
    auto near_plane_bottom_right =
      near_plane_bottom_left + 2 * near_plane_half_width * camera.get_right();
    auto far_plane_center =
      camera.get_position() + -camera.get_far_plane() * camera.get_direction();
    auto far_plane_half_height =
      tan_half_field_of_view * -camera.get_far_plane();
    auto far_plane_half_width =
      camera.get_aspect_ratio() * far_plane_half_height;
    auto far_plane_top_left = far_plane_center +
      far_plane_half_height * camera.get_orientation() -
      far_plane_half_width * camera.get_right();
    auto far_plane_top_right = 
      far_plane_top_left + 2 * far_plane_half_width * camera.get_right();
    auto far_plane_bottom_left =
      far_plane_top_left - 2 * far_plane_half_height * camera.get_orientation();
    auto far_plane_bottom_right =
      far_plane_bottom_left + 2 * far_plane_half_width * camera.get_right();
    m_planes[static_cast<std::size_t>(ClippingPlane::LEFT)] = make_plane(
      camera.get_position(), near_plane_bottom_left, near_plane_top_left);
    m_planes[static_cast<std::size_t>(ClippingPlane::RIGHT)] = make_plane(
      camera.get_position(), near_plane_top_right, near_plane_bottom_right);
    m_planes[static_cast<std::size_t>(ClippingPlane::BOTTOM)] = make_plane(
      camera.get_position(), near_plane_bottom_right, near_plane_bottom_left);
    m_planes[static_cast<std::size_t>(ClippingPlane::TOP)] = make_plane(
      camera.get_position(), near_plane_top_left, near_plane_top_right);
    m_planes[static_cast<std::size_t>(ClippingPlane::NEAR)] = make_plane(
      near_plane_top_left, near_plane_top_right, near_plane_bottom_right);
    m_planes[static_cast<std::size_t>(ClippingPlane::FAR)] = make_plane(
      far_plane_top_right, far_plane_top_left, far_plane_bottom_left);
  }
}

#endif
