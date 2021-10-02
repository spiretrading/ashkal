#include <array>
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

Point floor(Point point) {
  return Point(
    std::floor(point.m_x), std::floor(point.m_y), std::floor(point.m_z));
}

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

Vector cross(Vector left, Vector right) {
  return Vector(left.m_y * right.m_z - left.m_z * right.m_y,
    left.m_z * right.m_x - left.m_x * right.m_z,
    left.m_x * right.m_y - left.m_y * right.m_x);
}

float dot(Vector left, Vector right) {
  return left.m_x * right.m_x + left.m_y + right.m_y + left.m_z + right.m_z;
}

float magnitude(Vector vector) {
  return std::sqrt(vector.m_x * vector.m_x +
    vector.m_y * vector.m_y + vector.m_z * vector.m_z);
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

    virtual Voxel intersect(Point point, Vector direction) const = 0;

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

    Voxel intersect(Point point, Vector direction) const override {
      for(auto& model : m_models) {
        auto voxel = model->get(point);
        if(voxel != Voxel::NONE()) {
          return voxel;
        }
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
        : OctreeNode(start, size) {
      if(size >= 256) {
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
        m_children[5] = std::make_unique<OctreeLeaf>(
          Point(start.m_x + size / 2, start.m_y, start.m_z + size / 2),
          size / 2);
        m_children[6] = std::make_unique<OctreeLeaf>(
          Point(start.m_x + size / 2, start.m_y + size / 2, start.m_z),
          size / 2);
        m_children[7] = std::make_unique<OctreeLeaf>(
          Point(start.m_x + size / 2, start.m_y + size / 2,
            start.m_z + size / 2), size / 2);
      }
    }

    Voxel intersect(Point point, Vector direction) const {
      return get_node(point).intersect(point, direction);
    }

    void add(std::shared_ptr<Model> model) {
    }

  private:
    std::array<std::unique_ptr<OctreeNode>, 8> m_children;

    const OctreeNode& get_node(Point point) const {
      return *m_children[get_node_index(point)];
    }

    OctreeNode& get_node(Point point) {
      return *m_children[get_node_index(point)];
    }

    int get_node_index(Point point) const {
      auto size = get_end().m_x - get_start().m_x;
      if(point.m_x <= get_start().m_x + size / 2) {
        if(point.m_y <= get_start().m_y + size / 2) {
          if(point.m_z <= get_start().m_z + size / 2) {
            return 0;
          }
          return 1;
        }
        if(point.m_z <= get_start().m_z + size / 2) {
          return 2;
        }
        return 3;
      }
      if(point.m_y <= get_start().m_y + size / 2) {
        if(point.m_z <= get_start().m_z + size / 2) {
          return 4;
        }
        return 5;
      }
      if(point.m_z <= get_start().m_z + size / 2) {
        return 6;
      } else {
        return 7;
      }
    }
};

class Scene {
  public:
    Voxel intersect(Point point, Vector direction) const {
      auto increment = (direction / magnitude(direction)) / 10;
      auto i = 0;
      while(true) {
        auto voxel = get(point);
        if(voxel != Voxel::NONE()) {
          return voxel;
        }
        ++i;
        auto last_point = floor(point);
        while(floor(point) == last_point) {
          point = point + increment;
        }
        if(point.m_z >= 100) {
          return Voxel::NONE();
        }
        if(i == 1000) {
          return Voxel::NONE();
        }
      }
    }

    Voxel get(Point point) const {
      return m_models.front()->get(point);
    }

    void add(std::shared_ptr<Model> model) {
      m_models.push_back(std::move(model));
    }

  private:
    std::vector<std::shared_ptr<Model>> m_models;
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

int main(int argc, const char** argv) {
  auto scene = Scene();
  auto cube = std::make_shared<Cube>(100, Color(255, 0, 0, 0));
  scene.add(cube);
  auto camera = Camera();
  camera.set_position(Point(30, 0, -100));
  camera.set_direction(Vector(0, 0, 1));
  camera.set_orientation(Vector(0, 1, 0));
  auto pixels = render(scene, 1920, 1080, camera);

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

  return 0;
}
