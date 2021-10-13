#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#undef main
#include <GL/GL.h>
#include <GL/GLU.h>
#include <Windows.h>
#include <boost/compute.hpp>
#include <boost/compute/interop/opengl/context.hpp>
#include "Version.hpp"

using namespace boost;

struct Accelerator {
  compute::context m_context;
  compute::command_queue m_queue;

  Accelerator()
    : m_context(compute::opengl_create_shared_context()),
      m_queue(m_context, m_context.get_device()) {}
};

template<typename F>
auto profile(F&& f) {
  auto s = std::chrono::high_resolution_clock::now();
  if constexpr(std::is_same_v<decltype(f()), void>) {
    std::forward<F>(f)();
    auto e = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(e - s) << std::endl;
  } else {
    auto r = std::forward<F>(f)();
    auto e = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(e - s) << std::endl;
    return r;
  }
}

namespace {
const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}

unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

void generateBitmapImage (const unsigned char* image, int height, int width, const char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}
}

struct Color {
  std::uint8_t m_red;
  std::uint8_t m_green;
  std::uint8_t m_blue;
  std::uint8_t m_alpha;

  friend auto operator <=>(const Color&, const Color&) = default;
};

std::ostream& operator <<(std::ostream& out, Color color) {
  return out << "Color(" << static_cast<int>(color.m_red) << ' ' <<
    static_cast<int>(color.m_green) << ' ' << static_cast<int>(color.m_blue) <<
    ' ' << static_cast<int>(color.m_alpha) << ')';
}

struct Point {
  float m_x;
  float m_y;
  float m_z;

  friend auto operator <=>(const Point&, const Point&) = default;
};

std::ostream& operator <<(std::ostream& out, Point point) {
  return out <<
    "Point(" << point.m_x << ' ' << point.m_y << ' ' << point.m_z << ')';
}

struct Voxel {
  constexpr static Voxel NONE() {
    return Voxel(Color(0, 0, 0, 255));
  }

  Color m_color;

  friend auto operator <=>(const Voxel&, const Voxel&) = default;
};

std::ostream& operator <<(std::ostream& out, Voxel voxel) {
  return out <<
    "Voxel(" << voxel.m_color << ')';
}

struct Vector {
  float m_x;
  float m_y;
  float m_z;
};

std::ostream& operator <<(std::ostream& out, Vector vector) {
  return out <<
    "Vector(" << vector.m_x << ' ' << vector.m_y << ' ' << vector.m_z << ')';
}

Vector operator -(Vector vector) {
  return Vector(-vector.m_x, -vector.m_y, -vector.m_z);
}

Vector operator -(Point left, Point right) {
  return Vector(
    left.m_x - right.m_x, left.m_y - right.m_y, left.m_z - right.m_z);
}

Vector operator -(Vector left, Vector right) {
  return Vector(
    left.m_x - right.m_x, left.m_y - right.m_y, left.m_z - right.m_z);
}

Vector operator +(Vector left, Vector right) {
  return Vector(
    left.m_x + right.m_x, left.m_y + right.m_y, left.m_z + right.m_z);
}

Vector operator *(int left, Vector right) {
  return Vector(left * right.m_x, left * right.m_y, left * right.m_z);
}

Vector operator *(float left, Vector right) {
  return Vector(left * right.m_x, left * right.m_y, left * right.m_z);
}

Vector operator /(Vector left, int right) {
  return Vector(left.m_x / right, left.m_y / right, left.m_z / right);
}

Vector operator /(Vector left, float right) {
  return Vector(left.m_x / right, left.m_y / right, left.m_z / right);
}

Point operator +(Point left, Vector right) {
  return Point(
    left.m_x + right.m_x, left.m_y + right.m_y, left.m_z + right.m_z);
}

Point operator -(Point left, Vector right) {
  return left + -right;
}

Point floor(Point point) {
  return Point(
    std::floor(point.m_x), std::floor(point.m_y), std::floor(point.m_z));
}

Vector cross(Vector left, Vector right) {
  return Vector(left.m_y * right.m_z - left.m_z * right.m_y,
    left.m_z * right.m_x - left.m_x * right.m_z,
    left.m_x * right.m_y - left.m_y * right.m_x);
}

float dot(Vector left, Vector right) {
  return left.m_x * right.m_x + left.m_y * right.m_y + left.m_z * right.m_z;
}

float magnitude(Vector vector) {
  return std::sqrt(vector.m_x * vector.m_x +
    vector.m_y * vector.m_y + vector.m_z * vector.m_z);
}

Vector normalize(Vector vector) {
  return vector / magnitude(vector);
}

struct Ray {
  Point m_point;
  Vector m_direction;
};

Point point_at(const Ray& ray, float t) {
  return ray.m_point + t * ray.m_direction;
}

Point compute_boundary(const Ray& ray, Point start, int size) {
  auto x_distance = [&] {
    if(ray.m_direction.m_x == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_x > 0) {
      return start.m_x + size - ray.m_point.m_x;
    }
    return start.m_x - ray.m_point.m_x - 1;
  }();
  auto y_distance = [&] {
    if(ray.m_direction.m_y == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_y > 0) {
      return start.m_y + size - ray.m_point.m_y;
    }
    return start.m_y - ray.m_point.m_y - 1;
  }();
  auto z_distance = [&] {
    if(ray.m_direction.m_z == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_z > 0) {
      return start.m_z + size - ray.m_point.m_z;
    }
    return start.m_z - ray.m_point.m_z - 1;
  }();
  auto t = INFINITY;
  if(x_distance != INFINITY) {
    t = x_distance / ray.m_direction.m_x;
  }
  if(y_distance != INFINITY) {
    auto r = y_distance / ray.m_direction.m_y;
    if(r < t) {
      t = r;
    }
  }
  if(z_distance != INFINITY) {
    auto r = z_distance / ray.m_direction.m_z;
    if(r < t) {
      t = r;
    }
  }
  return point_at(ray, t);
}

bool contains(Point start, Point end, Point point) {
  return point.m_x >= start.m_x && point.m_x < end.m_x &&
    point.m_y >= start.m_y && point.m_y < end.m_y &&
    point.m_z >= start.m_z && point.m_z < end.m_z;
}

class Model {
  public:
    virtual ~Model() = default;

    // Exclusive.
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
      if(point.m_x >= 0 && point.m_y >= 0 && point.m_z >= 0 &&
          point.m_x < m_size && point.m_y < m_size && point.m_z < m_size) {
        return Voxel(m_color);
      }
      return Voxel::NONE();
    }

  private:
    int m_size;
    Color m_color;
};

class Sphere : public Model {
  public:
    Sphere(int radius, Color color)
      : m_radius(radius),
        m_inner(float(m_radius - 1), float(m_radius - 1), float(m_radius - 1)),
        m_color(color) {}

    Point end() const override {
      return Point(2 * static_cast<float>(m_radius) - 1,
        2 * static_cast<float>(m_radius) - 1,
        2 * static_cast<float>(m_radius) - 1);
    }

    Voxel get(Point point) const override {
      auto range = floor(point) - m_inner;
      if(dot(range, range) <= m_radius * m_radius) {
        return Voxel(m_color);
      }
      return Voxel::NONE();
    }

  private:
    int m_radius;
    Point m_inner;
    Color m_color;
};

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
      for(auto& model : m_models) {
        auto voxel = model->get(point);
        if(voxel != Voxel::NONE()) {
          return voxel;
        }
      }
      return Voxel::NONE();
    }

    Voxel intersect(Point& point, Vector direction) const override {
      if(m_models.empty()) {
        point = compute_boundary(Ray(point, direction), get_start(),
          static_cast<int>(get_end().m_x - get_start().m_x));
        return Voxel::NONE();
      }
      while(contains(get_start(), get_end(), point)) {
        auto voxel = get(point);
        if(voxel != Voxel::NONE()) {
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
      if(size >= 128) {
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
        m_children[5] = std::make_unique<OctreeLeaf>(Point(start.m_x + size / 2,
          start.m_y, start.m_z + size / 2), size / 2);
        m_children[6] = std::make_unique<OctreeLeaf>(Point(start.m_x + size / 2,
          start.m_y + size / 2, start.m_z), size / 2);
        m_children[7] = std::make_unique<OctreeLeaf>(Point(start.m_x + size / 2,
          start.m_y + size / 2, start.m_z + size / 2), size / 2);
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

class Scene {
  public:
    Scene()
      : m_root(Point(-2048, -2048, -2048), 4096) {}

    Voxel get(Point point) const {
      return m_root.get(point);
    }

    Voxel intersect(Point point, Vector direction) const {
      return m_root.intersect(point, direction);
    }

    void add(std::shared_ptr<Model> model) {
      m_root.add(model);
    }

  private:
    OctreeInternalNode m_root;
};

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
      m_direction = direction / magnitude(direction);
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

BOOST_COMPUTE_ADAPT_STRUCT(Color, Color, (m_red, m_green, m_blue, m_alpha));
BOOST_COMPUTE_ADAPT_STRUCT(Voxel, Voxel, (m_color));
BOOST_COMPUTE_ADAPT_STRUCT(Point, Point, (m_x, m_y, m_z));
BOOST_COMPUTE_ADAPT_STRUCT(Vector, Vector, (m_x, m_y, m_z));
BOOST_COMPUTE_ADAPT_STRUCT(Ray, Ray, (m_point, m_direction));

void intersect(const Scene& scene, compute::vector<Color>& pixels,
    int width, int height, Point camera, Vector top_left, Vector x_shift,
    Vector y_shift, Accelerator& accelerator) {
  static auto kernel = [&] {
    static auto source = compute::type_definition<Color>() +
      compute::type_definition<Voxel>() +
      compute::type_definition<Point>() +
      compute::type_definition<Vector>() +
      compute::type_definition<Ray>() +
      BOOST_COMPUTE_STRINGIZE_SOURCE(
        typedef struct {
          int m_width;
          int m_height;
          int m_depth;
          __global Voxel* m_points;
        } Scene;

        Voxel make_voxel(Color color) {
          Voxel voxel;
          voxel.m_color = color;
          return voxel;
        }

        Color make_color(unsigned char red, unsigned green, unsigned char blue,
            unsigned char alpha) {
          Color color;
          color.m_red = red;
          color.m_green = green;
          color.m_blue = blue;
          color.m_alpha = alpha;
          return color;
        }

        Point make_point(float x, float y, float z) {
          Point point;
          point.m_x = x;
          point.m_y = y;
          point.m_z = z;
          return point;
        }

        Point floor_point(Point point) {
          return make_point(
            floor(point.m_x), floor(point.m_y), floor(point.m_z));
        }

        Vector make_vector(float x, float y, float z) {
          Vector vector;
          vector.m_x = x;
          vector.m_y = y;
          vector.m_z = z;
          return vector;
        }

        Vector add_vector(Vector left, Vector right) {
          left.m_x += right.m_x;
          left.m_y += right.m_y;
          left.m_z += right.m_z;
          return left;
        }

        Vector sub_vector(Vector left, Vector right) {
          left.m_x -= right.m_x;
          left.m_y -= right.m_y;
          left.m_z -= right.m_z;
          return left;
        }

        Vector mul_int_vector(int left, Vector right) {
          right.m_x *= left;
          right.m_y *= left;
          right.m_z *= left;
          return right;
        }

        Vector mul_float_vector(float left, Vector right) {
          right.m_x *= left;
          right.m_y *= left;
          right.m_z *= left;
          return right;
        }

        Vector div_vector_float(Vector left, float right) {
          left.m_x /= right;
          left.m_y /= right;
          left.m_z /= right;
          return left;
        }

        Point add_point_vector(Point point, Vector vector) {
          point.m_x += vector.m_x;
          point.m_y += vector.m_y;
          point.m_z += vector.m_z;
          return point;
        }

        float magnitude(Vector vector) {
          return sqrt(vector.m_x * vector.m_x +
            vector.m_y * vector.m_y + vector.m_z * vector.m_z);
        }

        Vector norm(Vector vector) {
          return div_vector_float(vector, magnitude(vector));
        }

        bool is_equal_color(Color left, Color right) {
          return left.m_red == right.m_red && left.m_green == right.m_green &&
            left.m_blue == right.m_blue && left.m_alpha == right.m_alpha;
        }

        bool is_none_voxel(Voxel voxel) {
          return is_equal_color(voxel.m_color, make_color(0, 0, 0, 255));
        }

        Voxel get_voxel_from_scene(Scene scene, Point point) {
          point = floor_point(point);
          if(point.m_x < 0 || point.m_y < 0 || point.m_z < 0 ||
              point.m_x >= scene.m_width || point.m_y >= scene.m_height ||
              point.m_z >= scene.m_depth) {
            return make_voxel(make_color(0, 0, 0, 255));
          }
          size_t index = (size_t)(point.m_x) + scene.m_width *
            ((size_t)(point.m_y) + scene.m_height * (size_t)(point.m_z));
          return scene.m_points[index];
        }

        Point point_at(Ray ray, float t) {
          return add_point_vector(
            ray.m_point, mul_float_vector(t, ray.m_direction));
        }

        Point compute_boundary(Ray ray, Point start, int size) {
          float x_distance;
          if(ray.m_direction.m_x == 0) {
            x_distance = INFINITY;
          } else if(ray.m_direction.m_x > 0) {
            x_distance = start.m_x + size - ray.m_point.m_x;
          } else {
            x_distance = start.m_x - ray.m_point.m_x - 1;
          }
          float y_distance;
          if(ray.m_direction.m_y == 0) {
            y_distance = INFINITY;
          } else if(ray.m_direction.m_y > 0) {
            y_distance = start.m_y + size - ray.m_point.m_y;
          } else {
            y_distance = start.m_y - ray.m_point.m_y - 1;
          }
          float z_distance;
          if(ray.m_direction.m_z == 0) {
            z_distance = INFINITY;
          } else if(ray.m_direction.m_z > 0) {
            z_distance = start.m_z + size - ray.m_point.m_z;
          } else {
            z_distance = start.m_z - ray.m_point.m_z - 1;
          }
          float t = INFINITY;
          if(x_distance != INFINITY) {
            t = x_distance / ray.m_direction.m_x;
          }
          if(y_distance != INFINITY) {
            float r = y_distance / ray.m_direction.m_y;
            if(r < t) {
              t = r;
            }
          }
          if(z_distance != INFINITY) {
            float r = z_distance / ray.m_direction.m_z;
            if(r < t) {
              t = r;
            }
          }
          return point_at(ray, t);
        }

        bool contains(Point start, Point end, Point point) {
          return point.m_x >= start.m_x && point.m_x < end.m_x &&
            point.m_y >= start.m_y && point.m_y < end.m_y &&
            point.m_z >= start.m_z && point.m_z < end.m_z;
        }

        void print_point(Point point) {
          printf("Point(%f, %f, %f)", point.m_x, point.m_y, point.m_z);
        }

        void print_vector(Vector vector) {
          printf("Vector(%f, %f, %f)", vector.m_x, vector.m_y, vector.m_z);
        }

        Voxel trace(Scene scene, Ray ray) {
          while(contains(make_point(-64, -64, -2048), make_point(
              scene.m_width, scene.m_height, scene.m_depth), ray.m_point)) {
            Voxel voxel = get_voxel_from_scene(scene, ray.m_point);
            if(!is_none_voxel(voxel)) {
              return voxel;
            }
            ray.m_point = compute_boundary(ray, floor_point(ray.m_point), 1);
          }
          return make_voxel(make_color(0, 0, 0, 255));
        }

        __kernel void intersect(__global Voxel* points, int scene_width,
            int scene_height, int scene_depth, __global Color* pixels,
            int width, int height, Point camera, Vector top_left,
            Vector x_shift, Vector y_shift) {
          int i = get_global_id(0);
          int x = i % width;
          int y = i / width;
          Scene scene;
          scene.m_width = scene_width;
          scene.m_height = scene_height;
          scene.m_depth = scene_depth;
          scene.m_points = points;
          Vector direction = sub_vector(add_vector(
            top_left, mul_int_vector(x, x_shift)), mul_int_vector(y, y_shift));
          Ray ray;
          ray.m_point = add_point_vector(camera, direction);
          ray.m_direction = norm(direction);
          Voxel voxel = trace(scene, ray);
          if(is_none_voxel(voxel)) {
            pixels[i] = make_color(0, 0, 0, 0);
          } else {
            pixels[i] = voxel.m_color;
          }
        });
    auto cache =
      compute::program_cache::get_global_cache(accelerator.m_context);
    auto key = std::string("__intersect");
    auto program = cache->get_or_build(key, {}, source, accelerator.m_context);
    return program.create_kernel("intersect");
  }();
  auto SIZE = 64;
  auto host = std::vector<Voxel>();
  host.resize(SIZE * SIZE * SIZE, Voxel::NONE());
  auto s = compute::vector<Voxel>(SIZE * SIZE * SIZE, accelerator.m_context);
  profile([&] {
    for(auto x = 0; x < SIZE; ++x) {
      for(auto y = 0; y < SIZE; ++y) {
        for(auto z = 0; z < SIZE; ++z) {
          host[x + SIZE * (y + SIZE * z)] = scene.get(Point(
            static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
        }
      }
    }
    compute::copy(host.begin(), host.end(), s.begin(), accelerator.m_queue);
    kernel.set_arg(0, s.get_buffer());
    kernel.set_arg(1, SIZE);
    kernel.set_arg(2, SIZE);
    kernel.set_arg(3, SIZE);
    kernel.set_arg(4, pixels.get_buffer());
    kernel.set_arg(5, width);
    kernel.set_arg(6, height);
    kernel.set_arg(7, sizeof(Point), &camera);
    kernel.set_arg(8, sizeof(Vector), &top_left);
    kernel.set_arg(9, sizeof(Vector), &x_shift);
    kernel.set_arg(10, sizeof(Vector), &y_shift);
    accelerator.m_queue.enqueue_1d_range_kernel(kernel, 0, pixels.size(), 0);
    accelerator.m_queue.finish();
  });
}

std::vector<Color> render_gpu(const Scene& scene, Accelerator& accelerator,
    int width, int height, const Camera& camera) {
  auto aspect_ratio = static_cast<float>(height) / width;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2.f * aspect_ratio / height) * camera.get_orientation();
  auto device_pixels =
    compute::vector<Color>(width * height, accelerator.m_context);
  intersect(scene, device_pixels, width, height, camera.get_position(),
    top_left, x_shift, y_shift, accelerator);
  auto pixels = std::vector<Color>();
  pixels.resize(width * height);
  compute::copy(device_pixels.begin(), device_pixels.end(), pixels.begin(),
    accelerator.m_queue);
  return pixels;
}

std::vector<Color> render_cpu(
    const Scene& scene, int width, int height, const Camera& camera) {
  auto aspect_ratio = static_cast<float>(height) / width;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2.f * aspect_ratio / height) * camera.get_orientation();
  auto pixels = std::vector<Color>();
  pixels.reserve(width * height);
  profile([&] {
    for(auto y = 0; y < height; ++y) {
      for(auto x = 0; x < width; ++x) {
        auto direction = top_left + x * x_shift - y * y_shift;
        auto point = camera.get_position() + direction;
        auto voxel = scene.intersect(point, normalize(direction));
        if(voxel == Voxel::NONE()) {
          pixels.push_back(Color(0, 0, 0));
        } else {
          pixels.push_back(voxel.m_color);
        }
      }
    }
  });
  return pixels;
}

void save_bmp(const std::vector<Color>& pixels, std::string path) {
  constexpr int width = 1920;
  constexpr int height = 1080;
  auto image = new unsigned char[height][width][BYTES_PER_PIXEL];
  for(auto i = 0; i < height; ++i) {
    for(auto j = 0; j < width; ++j) {
      auto& color = pixels[j + width * i];
      image[height - i - 1][j][2] = color.m_red;
      image[height - i - 1][j][1] = color.m_green;
      image[height - i - 1][j][0] = color.m_blue;
    }
  }
  generateBitmapImage((unsigned char*) image, height, width, path.c_str());
}

void demo_gpu(Accelerator& accelerator) {
  auto scene = Scene();
  auto shape = std::make_shared<Sphere>(10, Color(255, 0, 0, 0));
  scene.add(shape);
  auto camera = Camera();
  camera.set_position(Point(9.5f, 9.5f, -10));
  camera.set_direction(Vector(0, 0, 1));
  camera.set_orientation(Vector(0, 1, 0));
  auto pixels = render_gpu(scene, accelerator, 1920, 1080, camera);
  save_bmp(pixels, "gpu.bmp");
}

void demo_cpu() {
  auto scene = Scene();
  auto shape = std::make_shared<Sphere>(10, Color(255, 0, 0, 0));
  scene.add(shape);
  auto camera = Camera();
  camera.set_position(Point(9.5f, 9.5f, -10));
  camera.set_direction(Vector(0, 0, 1));
  camera.set_orientation(Vector(0, 1, 0));
  auto pixels = render_cpu(scene, 1920, 1080, camera);
  save_bmp(pixels, "cpu.bmp");
}

auto make_shader() {
  auto programId = glCreateProgram();
  auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
  auto vertexShaderSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
    #version 140\n
    in vec2 LVertexPos2D;
    void main() {
      gl_Position = vec4(LVertexPos2D.x, LVertexPos2D.y, 0, 1);
    });
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);
  auto shaderCompiled = GL_FALSE;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompiled);
  if(shaderCompiled != GL_TRUE) {
    printf( "Unable to compile vertex shader %d!\n", vertexShader );
    throw std::exception();
  }
  glAttachShader(programId, vertexShader);
  auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  auto fragmentShaderSource = BOOST_COMPUTE_STRINGIZE_SOURCE(
    #version 140\n
    out vec4 LFragment;
    void main() {
      LFragment = vec4( 1.0, 1.0, 1.0, 1.0 );
    });
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  auto fShaderCompiled = GL_FALSE;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
  if(fShaderCompiled != GL_TRUE) {
    printf( "Unable to compile fragment shader %d!\n", fragmentShader );
    throw std::exception();
  }
  glAttachShader(programId, fragmentShader);
  glLinkProgram(programId);
  auto programSuccess = GL_TRUE;
  glGetProgramiv(programId, GL_LINK_STATUS, &programSuccess);
  if(programSuccess != GL_TRUE) {
    printf("Error linking program %d!\n", programId);
    throw std::exception();
  }
  auto vertexPos2DLocation = glGetAttribLocation(programId, "LVertexPos2D");
  if(vertexPos2DLocation == -1) {
    printf( "LVertexPos2D is not a valid glsl program variable!\n" );
    throw std::exception();
  }
  glClearColor(0.f, 0.f, 0.f, 1.f);
  GLfloat vertexData[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
  };
  GLuint indexData[] = { 0, 1, 2, 3 };
  auto vbo = GLuint();
  auto ibo = GLuint();
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData,
    GL_STATIC_DRAW);
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData,
    GL_STATIC_DRAW);
  return std::tuple(programId, vbo, ibo, vertexPos2DLocation);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR pCmdLine, int nCmdShow) {
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  auto window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(!window) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    return 1;
  }
  auto glContext = SDL_GL_CreateContext(window);
  if(glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW." << std::endl;
    return 1;
  }
  if(SDL_GL_SetSwapInterval(1) < 0) {
    std::cout <<
      "Warning: Unable to set VSync: " << SDL_GetError() << std::endl;
  }
  auto running = true;
  auto event = SDL_Event();
  auto gl_context = SDL_GL_GetCurrentContext();
  auto windowId = SDL_GetWindowID(window);
  auto accelerator = Accelerator();
//  render_gpu();
  auto [programId, vbo, ibo, vertexPos2DLocation] = make_shader();
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programId);
    glEnableVertexAttribArray(vertexPos2DLocation);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
      vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);
    glDisableVertexAttribArray(vertexPos2DLocation);
    glUseProgram(0);
    SDL_GL_SwapWindow(window);
  while(running) {
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_WINDOWEVENT:
          if(event.window.windowID == windowId)  {
            switch(event.window.event) {
              case SDL_WINDOWEVENT_CLOSE:
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
                break;
            }
          }
          break;
        case SDL_QUIT:
          running = false;
          break;
      }
    }
  }
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(glContext);
  SDL_Quit();
  return 0;
}
