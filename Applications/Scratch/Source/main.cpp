#include <cstdint>
#include <vector>
#include "Version.hpp"

struct Color {
  std::uint8_t red;
  std::uint8_t green;
  std::uint8_t blue;
  std::uint8_t alpha;
};

struct Point {
  int x;
  int y;
  int z;
};

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
      return Point(m_size, m_size, m_size);
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
  int x;
  int y;
  int z;
};

class Camera {
  public:
    Point get_position() const {
      return m_position;
    }

    void set_position(Point position) {
      m_position = position;
    }

    Vector get_orientation() const {
      return m_orientation;
    }

    void set_orientation(Vector orientation) {
      m_orientation = orientation;
    }

  private:
    Point m_position;
    Vector m_orientation;
};

std::vector<Color> render(
    const Scene& scene, int width, int height, const Camera& camera) {
  auto distance = height / 2;
  auto aspect_ratio = static_cast<double>(width) / height;
  for(auto y = 0; y < height; ++y) {
    for(auto x = 0; x < width; ++x) {
//      auto pixel_point = Point(x, y, 0);
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
  camera.set_orientation(Vector(0, 1, 0));
  auto pixels = render(scene, 1000, 1000, camera);
  return 0;
}
