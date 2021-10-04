#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "Version.hpp"

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

bool contains(Point start, Point end, Point point) {
  return point.m_x >= start.m_x && point.m_x < end.m_x &&
    point.m_y >= start.m_y && point.m_y < end.m_y &&
    point.m_z >= start.m_z && point.m_z < end.m_z;
}

struct Ray {
  Point m_point;
  Vector m_direction;
};

Point point_at(const Ray& ray, float t) {
  return ray.m_point + t * ray.m_direction;
}

struct PlaneSegment {
  Point m_point;
  Vector m_normal;
  int m_size;
};

bool contains(const PlaneSegment& plane, Point point) {
  auto tolerance = 0.000001f;
  if(plane.m_normal.m_x == 1) {
    return point.m_x >= plane.m_point.m_x - tolerance &&
      point.m_x < plane.m_point.m_x + 1 + tolerance &&
      point.m_y >= plane.m_point.m_y - tolerance &&
      point.m_y < plane.m_point.m_y + plane.m_size + tolerance &&
      point.m_z >= plane.m_point.m_z - tolerance &&
      point.m_z < plane.m_point.m_z + plane.m_size + tolerance;
  } else if(plane.m_normal.m_y == 1) {
    return point.m_x >= plane.m_point.m_x - tolerance &&
      point.m_x < plane.m_point.m_x + plane.m_size + tolerance &&
      point.m_y >= plane.m_point.m_y - tolerance &&
      point.m_y < plane.m_point.m_y + 1 + tolerance &&
      point.m_z >= plane.m_point.m_z - tolerance &&
      point.m_z < plane.m_point.m_z + plane.m_size + tolerance;
  }
  return point.m_x >= plane.m_point.m_x - tolerance &&
    point.m_x < plane.m_point.m_x + plane.m_size + tolerance &&
    point.m_y >= plane.m_point.m_y - tolerance &&
    point.m_y < plane.m_point.m_y + plane.m_size + tolerance &&
    point.m_z >= plane.m_point.m_z - tolerance &&
    point.m_z < plane.m_point.m_z + 1 + tolerance;
}

Vector flip(Vector normal) {
  if(normal.m_x == 1) {
    return Vector(0, 1, 0);
  } else if(normal.m_y == 1) {
    return Vector(0, 0, 1);
  }
  return Vector(1, 0, 0);
}

float intersect(const Ray& ray, const PlaneSegment& plane) {
  auto d = dot(ray.m_direction, plane.m_normal);
  auto n = dot(plane.m_point - ray.m_point, plane.m_normal);
  if(d == 0) {
    if(n != 0) {
      return NAN;
    }
    auto n1 = flip(plane.m_normal);
    auto p = PlaneSegment(plane.m_point - n1, n1, plane.m_size);
    auto t = intersect(ray, p);
    if(!std::isnan(t)) {
      return t;
    }
    auto n2 = flip(n1);
    p = PlaneSegment(plane.m_point - n2, n2, plane.m_size);
    t = intersect(ray, p);
    if(!std::isnan(t)) {
      return t;
    }
    p = PlaneSegment(plane.m_point + plane.m_size * n1, n1, plane.m_size);
    t = intersect(ray, p);
    if(!std::isnan(t)) {
      return t;
    }
    p = PlaneSegment(plane.m_point + plane.m_size * n2, n2, plane.m_size);
    return intersect(ray, p);
  }
  if(std::signbit(n) != std::signbit(d)) {
    return NAN;
  }
  auto t = n / d;
  if(t >= 0 && contains(plane, point_at(ray, t))) {
    return t;
  }
  return NAN;
}

Point compute_boundary(const Ray& ray, Point start, int size) {
  auto x_distance = [&] {
    if(ray.m_direction.m_x == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_x > 0) {
      return start.m_x + size + 1 - ray.m_point.m_x;
    }
    return start.m_x - 1 - ray.m_point.m_x;
  }();
  auto y_distance = [&] {
    if(ray.m_direction.m_y == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_y > 0) {
      return start.m_y + size + 1 - ray.m_point.m_y;
    }
    return start.m_y - 1 - ray.m_point.m_y;
  }();
  auto z_distance = [&] {
    if(ray.m_direction.m_z == 0) {
      return INFINITY;
    }
    if(ray.m_direction.m_z > 0) {
      return start.m_z + size + 1 - ray.m_point.m_z;
    }
    return start.m_z - 1 - ray.m_point.m_z;
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

Point intersect_plane(
    Point point, Vector direction, Point start, Vector normal) {
  auto d = dot(start - point, normal) / dot(direction, normal);
  return point + d * direction;
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
          point.m_x < 10 && point.m_y < 10 && point.m_z < m_size) {
        return Voxel(Color(0, 0, 255, 0));
      }
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

class OctreeNode {
  public:
    virtual ~OctreeNode() = default;

    Point get_start() const {
      return m_start;
    }

    Point get_end() const {
      return m_end;
    }

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

    Voxel get(Point point) const {
      for(auto& model : m_models) {
        auto voxel = model->get(point);
        if(voxel != Voxel::NONE()) {
          return voxel;
        }
      }
      return Voxel::NONE();
    }

    static inline auto intersections = 0;

    Voxel intersect(Point& point, Vector direction) const override {
      if(m_models.empty()) {
        point = compute_boundary(Ray(point, direction), get_start(),
          static_cast<int>(get_end().m_x - get_start().m_x));
        return Voxel::NONE();
      }
      while(contains(get_start(), get_end(), point)) {
        ++intersections;
        auto voxel = get(point);
        if(voxel != Voxel::NONE()) {
          return voxel;
        }
        point = compute_boundary(Ray(point, direction), point, 1);
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

std::vector<Color> render(
    const Scene& scene, int width, int height, const Camera& camera) {
  auto pixels = std::vector<Color>();
  pixels.reserve(width * height);
  auto aspect_ratio = static_cast<float>(height) / width;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left_direction =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2 * aspect_ratio / height) * camera.get_orientation();
  for(auto y = 0; y < height; ++y) {
    for(auto x = 0; x < width; ++x) {
      auto direction = top_left_direction + x * x_shift - y * y_shift;
      auto point = camera.get_position() + direction;
      auto voxel = scene.intersect(point, direction);
      if(voxel == Voxel::NONE()) {
        pixels.push_back(Color(0, 0, 0));
      } else {
        pixels.push_back(voxel.m_color);
      }
    }
  }
  return pixels;
}

void demo_scene() {
  auto scene = Scene();
  auto cube = std::make_shared<Cube>(100, Color(255, 0, 0, 0));
  scene.add(cube);
  auto camera = Camera();
  camera.set_position(Point(30, 0, -1000));
  camera.set_direction(Vector(0, 0, 1));
  camera.set_orientation(Vector(0, 1, 0));
  auto s = std::chrono::high_resolution_clock::now();
  auto pixels = render(scene, 1920, 1080, camera);
  auto e = std::chrono::high_resolution_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(e - s) << std::endl;
  std::cout << OctreeLeaf::intersections << std::endl;

    constexpr int width = 1920;
    constexpr int height = 1080;
    auto image = new unsigned char[height][width][BYTES_PER_PIXEL];
    auto imageFileName = "bitmapImage.bmp";

    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            image[height - i - 1][j][2] = (unsigned char) ( pixels[j + width * i].m_red);             ///red
            image[height - i - 1][j][1] = (unsigned char) ( pixels[j + width * i].m_green );              ///green
            image[height - i - 1][j][0] = (unsigned char) ( pixels[j + width * i].m_blue ); ///blue
        }
    }

    generateBitmapImage((unsigned char*) image, height, width, imageFileName);
    printf("Image generated!!");

}

int main(int argc, const char** argv) {
  demo_scene();
/*
  auto d = Vector(2, 1, 1);
  auto ray = Ray(Point(0, 0, 0), d / magnitude(d));
  auto p = compute_boundary(ray, Point(0, 0, 0), 1);
  std::cout << p;
*/
  return 0;
}
