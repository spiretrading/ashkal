#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <boost/compute.hpp>
#include <Windows.h>
#include <boost/compute/interop/opengl/acquire.hpp>
#include <boost/compute/interop/opengl/context.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include "Ashkal/AmbientLight.hpp"
#include "Ashkal/Camera.hpp"
#include "Ashkal/Color.hpp"
#include "Ashkal/Cube.hpp"
#include "Ashkal/DirectionalLight.hpp"
#include "Ashkal/Matrix.hpp"
#include "Ashkal/Model.hpp"
#include "Ashkal/Point.hpp"
#include "Ashkal/Sphere.hpp"
#include "Ashkal/Vector.hpp"
#include "Ashkal/Voxel.hpp"
#include "Version.hpp"

using namespace Ashkal;
using namespace boost;

struct Accelerator {
  compute::context m_context;
  compute::command_queue m_queue;

  Accelerator()
    : m_context(compute::opengl_create_shared_context()),
      m_queue(m_context, m_context.get_device()) {}
};

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

    AmbientLight get_ambient_light() const {
      return m_ambient_light;
    }

    void set(AmbientLight light) {
      m_ambient_light = light;
    }

    DirectionalLight get_directional_light() const {
      return m_directional_light;
    }

    void set(DirectionalLight light) {
      m_directional_light = light;
    };

    void add(std::shared_ptr<Model> model) {
      m_root.add(model);
    }

  private:
    AmbientLight m_ambient_light;
    DirectionalLight m_directional_light;
    OctreeInternalNode m_root;
};

BOOST_COMPUTE_ADAPT_STRUCT(Ray, Ray, (m_point, m_direction));

void intersect(const Scene& scene, compute::opengl_texture& texture, int width,
    int height, Point camera, Vector top_left, Vector x_shift, Vector y_shift,
    Accelerator& accelerator) {
  static auto kernel = [&] {
    static auto source = compute::type_definition<Color>() +
      compute::type_definition<Voxel>() +
      compute::type_definition<Point>() +
      compute::type_definition<Vector>() +
      compute::type_definition<Ray>() +
      compute::type_definition<AmbientLight>() +
      compute::type_definition<DirectionalLight>() +
      COLOR_CL_SOURCE +
      POINT_CL_SOURCE +
      VECTOR_CL_SOURCE +
      VOXEL_CL_SOURCE +
      AMBIENT_LIGHT_CL_SOURCE +
      DIRECTIONAL_LIGHT_CL_SOURCE +
      BOOST_COMPUTE_STRINGIZE_SOURCE(
        typedef struct {
          int m_width;
          int m_height;
          int m_depth;
          __global Voxel* m_points;
        } Scene;

        typedef struct {
          Voxel m_voxel;
          Point m_position;
        } VoxelIntersection;

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
            x_distance = start.m_x - ray.m_point.m_x - 0.001f;
          }
          float y_distance;
          if(ray.m_direction.m_y == 0) {
            y_distance = INFINITY;
          } else if(ray.m_direction.m_y > 0) {
            y_distance = start.m_y + size - ray.m_point.m_y;
          } else {
            y_distance = start.m_y - ray.m_point.m_y - 0.001f;
          }
          float z_distance;
          if(ray.m_direction.m_z == 0) {
            z_distance = INFINITY;
          } else if(ray.m_direction.m_z > 0) {
            z_distance = start.m_z + size - ray.m_point.m_z;
          } else {
            z_distance = start.m_z - ray.m_point.m_z - 0.001f;
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

        VoxelIntersection trace(Scene scene, Ray ray) {
          while(contains(make_point(-64, -64, -2048), make_point(
              scene.m_width, scene.m_height, scene.m_depth), ray.m_point)) {
            Voxel voxel = get_voxel_from_scene(scene, ray.m_point);
            if(!is_none_voxel(voxel)) {
              VoxelIntersection intersection;
              intersection.m_voxel = voxel;
              intersection.m_position = ray.m_point;
              return intersection;
            }
            ray.m_point = compute_boundary(ray, floor_point(ray.m_point), 1);
          }
          VoxelIntersection intersection;
          intersection.m_voxel = make_voxel(make_color(0, 0, 0, 255));
          intersection.m_position = make_point(0, 0, 0);
          return intersection;
        }

        __kernel void intersect(__global Voxel* points, int scene_width,
            int scene_height, int scene_depth, __write_only image2d_t pixels,
            Point camera, Vector top_left, Vector x_shift, Vector y_shift,
            AmbientLight ambient_light, DirectionalLight directional_light) {
          int x = get_global_id(0);
          int y = get_global_id(1);
          int width = get_global_size(0);
          int height = get_global_size(1);
          Scene scene;
          scene.m_width = scene_width;
          scene.m_height = scene_height;
          scene.m_depth = scene_depth;
          scene.m_points = points;
          Vector direction = sub_vector(add_vector(
            top_left, mul_int_vector(x, x_shift)), mul_int_vector(y, y_shift));
          Ray ray;
          ray.m_point = add_point_vector(camera, direction);
          ray.m_direction = normalize_vector(direction);
          VoxelIntersection intersection = trace(scene, ray);
          if(is_none_voxel(intersection.m_voxel)) {
            write_imagef(pixels, (int2)(x, y), (float4)(0.f, 0.f, 0.f, 0.f));
          } else {
            Color shaded_color =
              apply_ambient_light(ambient_light, intersection.m_voxel.m_color);
            shaded_color = add_color(shaded_color,
              apply_directional_light(directional_light,
                intersection.m_position, intersection.m_voxel.m_color));
            write_imagef(pixels, (int2)(x, y), (float4)(
              shaded_color.m_red / 255.f, shaded_color.m_green / 255.f,
              shaded_color.m_blue / 255.f, shaded_color.m_alpha / 255.f));
          }
        });
    auto cache =
      compute::program_cache::get_global_cache(accelerator.m_context);
    auto key = std::string("__intersect");
    auto program = cache->get_or_build(key, {}, source, accelerator.m_context);
    return program.create_kernel("intersect");
  }();
  static auto SIZE = 64;
  static auto s = [&] {
    auto s = compute::vector<Voxel>(SIZE * SIZE * SIZE, accelerator.m_context);
    auto host = std::vector<Voxel>();
    host.resize(SIZE * SIZE * SIZE, Voxel::NONE());
    for(auto x = 0; x < SIZE; ++x) {
      for(auto y = 0; y < SIZE; ++y) {
        for(auto z = 0; z < SIZE; ++z) {
          host[x + SIZE * (y + SIZE * z)] = scene.get(Point(
            static_cast<float>(x), static_cast<float>(y),
            static_cast<float>(z)));
        }
      }
    }
    compute::copy(host.begin(), host.end(), s.begin(), accelerator.m_queue);
    return s;
  }();
  kernel.set_arg(0, s.get_buffer());
  kernel.set_arg(1, SIZE);
  kernel.set_arg(2, SIZE);
  kernel.set_arg(3, SIZE);
  kernel.set_arg(4, texture);
  kernel.set_arg(5, sizeof(Point), &camera);
  kernel.set_arg(6, sizeof(Vector), &top_left);
  kernel.set_arg(7, sizeof(Vector), &x_shift);
  kernel.set_arg(8, sizeof(Vector), &y_shift);
  auto ambient_light = scene.get_ambient_light();
  kernel.set_arg(9, sizeof(AmbientLight), &ambient_light);
  auto directional_light = scene.get_directional_light();
  kernel.set_arg(10, sizeof(DirectionalLight), &directional_light);
  glFinish();
  compute::opengl_enqueue_acquire_gl_objects(1, &texture.get(),
    accelerator.m_queue);
  accelerator.m_queue.enqueue_nd_range_kernel(kernel, 2, nullptr,
    compute::dim(width, height).data(), nullptr);
  compute::opengl_enqueue_release_gl_objects(1, &texture.get(),
    accelerator.m_queue);
  accelerator.m_queue.finish();
}

void render_gpu(const Scene& scene, Accelerator& accelerator,
    compute::opengl_texture& texture, int width, int height,
    const Camera& camera) {
  auto aspect_ratio = static_cast<float>(height) / width;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2.f * aspect_ratio / height) * camera.get_orientation();
  intersect(scene, texture, width, height, camera.get_position(), top_left,
    x_shift, y_shift, accelerator);
}

void render_cpu(const Scene& scene, Accelerator& accelerator,
    compute::opengl_texture& texture, GLuint texture_id, int width, int height,
    const Camera& camera) {
  auto aspect_ratio = static_cast<float>(height) / width;
  auto roll = cross(camera.get_orientation(), camera.get_direction());
  auto top_left =
    camera.get_direction() - roll + aspect_ratio * camera.get_orientation();
  auto x_shift = (2.f / width) * roll;
  auto y_shift = (2.f * aspect_ratio / height) * camera.get_orientation();
  auto pixels = std::vector<Color>();
  pixels.reserve(width * height);
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
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
    GL_UNSIGNED_BYTE, pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

auto make_shader(int width, int height) {
  auto texture_id = GLuint();
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture_id;
}

auto render_text(const std::string& message, SDL_Color color, int font_size) {
  auto texture = GLuint(0);
  auto font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", font_size);
  if(!font) {
    return std::tuple(0, 0, texture);
  }
  auto surface = TTF_RenderText_Blended(font, message.c_str(), color);
  if(!surface) {
    TTF_CloseFont(font);
    return std::tuple(0, 0, texture);
  }
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  auto width = surface->w;
  auto height = surface->h;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
    GL_UNSIGNED_BYTE, surface->pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
  SDL_FreeSurface(surface);
  TTF_CloseFont(font);
  return std::tuple(width, height, texture);
}

void draw_text(const std::string& text, int size, int x, int y,
    const SDL_Color& color) {
  auto [width, height, texture] = render_text(text, color, size);
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glTexCoord2f(0.f, 0.f);
  glVertex2f(0.f, 0.f);
  glTexCoord2f(1.f, 0.f);
  glVertex2f(static_cast<float>(width), 0.f);
  glTexCoord2f(1.f, 1.f);
  glVertex2f(static_cast<float>(width), static_cast<float>(height));
  glTexCoord2f(0.f, 1.f);
  glVertex2f(0.f, static_cast<float>(height));
  glEnd();
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &texture);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR pCmdLine, int nCmdShow) {
  freopen("stdout.log", "w", stdout);
  freopen("stderr.log", "w", stderr);
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  TTF_Init();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  auto width = 1920;
  auto height = 1080;
  auto window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, width, height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(!window) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    return 1;
  }
  auto gl_context = SDL_GL_CreateContext(window);
  if(glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW." << std::endl;
    return 1;
  }
  if(SDL_GL_SetSwapInterval(1) < 0) {
    std::cout <<
      "Warning: Unable to set VSync: " << SDL_GetError() << std::endl;
    return 1;
  }
  auto accelerator = Accelerator();
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, height, 0.0, 1.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  auto texture_id = make_shader(width, height);
  auto texture =
    compute::opengl_texture(accelerator.m_context, GL_TEXTURE_2D, 0, texture_id,
      compute::opengl_texture::mem_flags::write_only);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.f, 0.f, 0.f);
  auto scene = Scene();
  scene.set(AmbientLight(Color(255, 255, 255, 0), 0.5));
  scene.set(DirectionalLight(Vector(0, 0, -1), Color(255, 255, 255, 0), 1.f));
  auto shape = std::make_shared<Sphere>(10, Color(255, 0, 0, 0));
  scene.add(shape);
  auto camera =
    Camera(Point(9.5f, 9.5f, -29), Vector(0, 0, 1), Vector(0, 1, 0));
  auto running = true;
  auto event = SDL_Event();
  auto window_id = SDL_GetWindowID(window);
  auto frames = 0;
  auto start = std::chrono::high_resolution_clock::now();
  auto last_mouse_x = std::numeric_limits<int>::min();
  auto last_mouse_y = std::numeric_limits<int>::min();
  while(running) {
    glClear(GL_COLOR_BUFFER_BIT);
    ++frames;
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_WINDOWEVENT:
          if(event.window.windowID == window_id) {
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
    auto state = SDL_GetKeyboardState(nullptr);
    if(state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
      move_forward(camera, 1 / 10.f);
    } else if(state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
      move_backward(camera, 1 / 10.f);
    }
    if(state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
      move_left(camera, 1 / 10.f);
    } else if(state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
      move_right(camera, 1 / 10.f);
    }
    auto mouse_x = 0;
    auto mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if(last_mouse_x != std::numeric_limits<int>::min() &&
        !state[SDL_SCANCODE_LALT] && !state[SDL_SCANCODE_RALT]) {
      auto delta_x = ((mouse_x - last_mouse_x) / (width / 2.f)) * (std::numbers::pi_v<float> / 2);
      auto delta_y = ((mouse_y - last_mouse_y) / (height / 2.f)) * (std::numbers::pi_v<float> / 2);
      tilt(camera, delta_x, delta_y);
    }
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    render_gpu(scene, accelerator, texture, width, height, camera);
//    render_cpu(scene, accelerator, texture, texture_id, width, height, camera);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(static_cast<float>(width), 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(static_cast<float>(width), static_cast<float>(height));
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, static_cast<float>(height));
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    auto info = "Position: " +
      lexical_cast<std::string>(camera.get_position()) + "\n";
    info += "Mouse: (" + lexical_cast<std::string>(mouse_x) + ", " +
      lexical_cast<std::string>(mouse_y) + ")\n";
    info += "Direction: " + lexical_cast<std::string>(camera.get_direction()) +
      "\n";
    info += "Orientation: " +
      lexical_cast<std::string>(camera.get_orientation()) + "\n";
    draw_text(info, 12, 0, 0, SDL_Color{.g=255, .a=255});
    SDL_GL_SwapWindow(window);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << (frames / std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count()) << std::endl;
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  glDeleteTextures(1, &texture_id);
  SDL_Quit();
  return 0;
}
