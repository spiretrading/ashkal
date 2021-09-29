#include <cstdint>
#include <iostream>
#include <vector>
#include "Version.hpp"

struct Color {
  std::uint8_t red;
  std::uint8_t green;
  std::uint8_t blue;
  std::uint8_t alpha;
};

struct Point {
  float x;
  float y;
  float z;
};

std::ostream& operator <<(std::ostream& out, Point point) {
  return out << "Point(" << point.x << ' ' << point.y << ' ' << point.z << ')';
}

struct Voxel {
  Color color;
};

class Model {
  public:
    virtual ~Model() = default;

    virtual Point end() const = 0;

    virtual Voxel get(Point point) const = 0;
};

class Cube : public Model {
  public:
    Cube(int size, Color color)
      : m_size(size),
        m_color(color) {}

    Point end() const override {
      return Point(static_cast<float>(m_size), static_cast<float>(m_size),
        static_cast<float>(m_size));
    }

    Voxel get(Point point) const override {
      return Voxel(m_color);
    }

  private:
    int m_size;
    Color m_color;
};

class Scene {
  public:
    Voxel get(Point point) const {
      return {};
    }

    void add(const Model& model) {}
};

struct Vector {
  float x;
  float y;
  float z;
};

std::ostream& operator <<(std::ostream& out, Vector vector) {
  return out <<
    "Vector(" << vector.x << ' ' << vector.y << ' ' << vector.z << ')';
}

Vector operator -(Vector left, Vector right) {
  return Vector(left.x - right.x, left.y - right.y, left.z - right.z);
}

Vector operator +(Vector left, Vector right) {
  return Vector(left.x + right.x, left.y + right.y, left.z + right.z);
}

Vector operator *(int left, Vector right) {
  return Vector(left * right.x, left * right.y, left * right.z);
}

Vector operator *(float left, Vector right) {
  return Vector(left * right.x, left * right.y, left * right.z);
}

Point operator +(Point left, Vector right) {
  return Point(left.x + right.x, left.y + right.y, left.z + right.z);
}

Vector cross(Vector left, Vector right) {
  return Vector(left.y * right.z - left.z * right.y,
    left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x);
}

class Camera {
  public:
    Point get_position() const {
      return m_position;
    }

    void set_position(Point position) {
      m_position = position;
    }

    Vector get_direction() const {
      return m_direction;
    }

    void set_direction(Vector direction) {
      m_direction = direction;
    }

    Vector get_orientation() const {
      return m_orientation;
    }

    void set_orientation(Vector orientation) {
      m_orientation = orientation;
    }

  private:
    Point m_position;
    Vector m_direction;
    Vector m_orientation;
};

std::vector<Color> render(
    const Scene& scene, int width, int height, const Camera& camera) {
  auto aspect_ratio = static_cast<float>(width) / height;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left_direction =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2 * aspect_ratio / height) * camera.get_orientation();
  for(auto x = 0; x < width; ++x) {
    for(auto y = 0; y < height; ++y) {
      auto direction = top_left_direction + x * x_shift - y * y_shift;
      auto point = camera.get_position() + direction;
      if(x == 0) {
        std::cout << x << " " << y << ": " << point << std::endl;
      }
    }
  }
  return {};
}

int main(int argc, const char** argv) {
  auto scene = Scene();
  auto cube = Cube(100, Color(255, 0, 0));
  scene.add(cube);
  auto camera = Camera();
  camera.set_position(Point(-50, -50, -50));
  camera.set_direction(Vector(0, 0, 1));
  camera.set_orientation(Vector(0, 1, 0));
  auto pixels = render(scene, 100, 100, camera);
  return 0;
}
