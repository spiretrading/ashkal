#include <iostream>
#include <limits>
#include <numbers>
#include <utility>
#include <vector>
#include <immintrin.h>
#include <boost/compute.hpp>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <boost/compute/interop/opengl/acquire.hpp>
#include <boost/compute/interop/opengl/context.hpp>
#include <boost/compute/interop/opengl/opengl_texture.hpp>
#include "Ashkal/Camera.hpp"
#include "Ashkal/SceneElement.hpp"
#include "Version.hpp"

using namespace Ashkal;
using namespace boost;

Point transform(const Point& point, const Camera& camera) {
  auto rel = point - camera.get_position();
  return Point(dot(rel, camera.get_right()),
    dot(rel, camera.get_orientation()), dot(rel, -camera.get_direction()));
}

std::pair<int, int> project_to_screen(
    const Point& point, int width, int height) {
  const auto NEAR_PLANE = 0.001f;
  const auto EYE_ORIGIN = 1;
  auto camera = Point(point.m_x, point.m_y, point.m_z - EYE_ORIGIN);
  if(camera.m_z >= 0) {
    camera.m_z = -NEAR_PLANE;
  }
  auto perspective = 1 / -camera.m_z;
  auto aspect_ratio = float(width) / height;
  auto normalized_x = (camera.m_x * perspective) / aspect_ratio;
  auto normalized_y = camera.m_y * perspective;
  auto fx = (normalized_x + 1) * 0.5f * width;
  auto fy = (1 - (normalized_y + 1) * 0.5f) * height;
  auto screen_x = int(std::round(fx));
  auto screen_y = int(std::round(fy));
  return std::pair(screen_x, screen_y);
}

float compute_edge(const std::pair<int, int>& p1,
    const std::pair<int, int>& p2, const std::pair<int, int>& p) {
  return (p2.first - p1.first) * (p.second - p1.second) -
    (p2.second - p1.second) * (p.first - p1.first);
}

Color lerp(Color a, Color b, float t) {
  return Color(std::lerp(a.m_red, b.m_red, t),
    std::lerp(a.m_green, b.m_green, t), std::lerp(a.m_blue, b.m_blue, t),
    std::lerp(a.m_alpha, b.m_alpha, t));
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Vertex& a, const Vertex& b,
    const Vertex& c, const Point& camera_a, const Point& camera_b,
    const Point& camera_c, int width, int height) {
  auto screen_a = project_to_screen(camera_a, width, height);
  auto screen_b = project_to_screen(camera_b, width, height);
  auto screen_c = project_to_screen(camera_c, width, height);
  auto min_x =
    std::max(0, std::min({screen_a.first, screen_b.first, screen_c.first}));
  auto max_x = std::min(
    width - 1, std::max({screen_a.first, screen_b.first, screen_c.first}));
  auto min_y =
    std::max(0, std::min({screen_a.second, screen_b.second, screen_c.second}));
  auto max_y = std::min(
    height - 1, std::max({screen_a.second, screen_b.second, screen_c.second}));
  auto depth_a = -camera_a.m_z;
  auto depth_b = -camera_b.m_z;
  auto depth_c = -camera_c.m_z;
  for(auto y = min_y; y <= max_y; ++y) {
    for(auto x = min_x; x <= max_x; ++x) {
      auto p = std::pair(x, y);
      auto w0 = compute_edge(screen_b, screen_c, p);
      auto w1 = compute_edge(screen_c, screen_a, p);
      auto w2 = compute_edge(screen_a, screen_b, p);
      if(w0 >= 0 && w1 >= 0 && w2 >= 0) {
        auto sum = w0 + w1 + w2;
        auto z_interpolated =
          (w0 * depth_a + w1 * depth_b + w2 * depth_c) / sum;
        auto index = y * width + x;
        if(z_interpolated < depth_buffer[index]) {
          depth_buffer[index] = z_interpolated;
          auto color = lerp(a.m_color, b.m_color, w1 / sum);
          auto final_color = lerp(color, c.m_color, w2 / sum);
          auto pixel =
            (std::uint32_t(final_color.m_red) << 24) |
            (std::uint32_t(final_color.m_green) << 16) |
            (std::uint32_t(final_color.m_blue) << 8) |
            std::uint32_t(final_color.m_alpha);
          frame_buffer[index] = pixel;
        }
      }
    }
  }
}

bool isInFront(const Point& P) {
    return P.m_z < .5f;
}

Point intersectNearPlanePoint(const Point& A, const Point& B) {
    const auto NEAR_EPS = 1e-5f;
    float t = (A.m_z - .5f) / (A.m_z - B.m_z);
    Point I;
    I.m_x = A.m_x + t * (B.m_x - A.m_x);
    I.m_y = A.m_y + t * (B.m_y - A.m_y);
    I.m_z = .5f - NEAR_EPS;
    return I;
}

Color intersectNearPlaneColor(const Point& Apos, const Color& Acol,
  const Point& Bpos, const Color& Bcol) {
  return Acol;
/*
    float t = Apos.m_z / (Apos.m_z - Bpos.m_z);
    Color cA = Acol * (1.0f - t);
    Color cB = Bcol * (t);
    return { uint8_t(std::round(cA.m_red   + cB.m_red)),
             uint8_t(std::round(cA.m_green + cB.m_green)),
             uint8_t(std::round(cA.m_blue  + cB.m_blue)),
             uint8_t(std::round(cA.m_alpha + cB.m_alpha)) };
*/
}

int clip(const Vertex& a, const Vertex& b, const Vertex& c, Point camera_a,
    Point camera_b, Point camera_c, std::array<Vertex, 4>& clipped_vertices,
    std::array<Point, 4>& clipped_points) {
  auto n = 0;
  auto processEdge = [&] (
      const Vertex& v0, const Vertex& v1, const Point& p0, const Point& p1) {
    auto color_0 = v0.m_color;
    auto color_1 = v1.m_color;
    auto in0 = isInFront(p0);
    auto in1 = isInFront(p1);
    if(in0 && in1) {
      clipped_points[n] = p1;
      clipped_vertices[n].m_color = color_1;
      ++n;
    } else if(in0 && !in1) {
      clipped_points[n] = intersectNearPlanePoint(p0, p1);
      clipped_vertices[n].m_color =
        intersectNearPlaneColor(p0, color_0, p1, color_1);
      ++n;
    } else if(!in0 && in1) {
      clipped_points[n] = intersectNearPlanePoint(p0, p1);
      clipped_vertices[n].m_color =
        intersectNearPlaneColor(p0, color_0, p1, color_1);
      ++n;
      clipped_points[n] = p1;
      clipped_vertices[n].m_color = color_1;
      ++n;
    }
  };
  processEdge(a, b, camera_a, camera_b);
  processEdge(b, c, camera_b, camera_c);
  processEdge(c, a, camera_c, camera_a);
  return n;
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices, const MeshTriangle& triangle,
    const Matrix& transformation, int width, int height) {
  auto& a = vertices[triangle.m_a];
  auto& b = vertices[triangle.m_b];
  auto& c = vertices[triangle.m_c];
  auto camera_a = transform(transformation * a.m_position, camera);
  auto camera_b = transform(transformation * b.m_position, camera);
  auto camera_c = transform(transformation * c.m_position, camera);
  if(camera_a.m_z < 0 && camera_b.m_z < 0 && camera_c.m_z < 0) {
    render(frame_buffer, depth_buffer, a, b, c, camera_a, camera_b, camera_c,
      width, height);
    return;
  }
  auto clipped_points = std::array<Point, 4>();
  auto clipped_vertices = std::array<Vertex, 4>();
  auto clipped_count = clip(
    a, b, c, camera_a, camera_b, camera_c, clipped_vertices, clipped_points);
  if(clipped_count < 3) {
    return;
  }
  render(frame_buffer, depth_buffer, clipped_vertices[0], clipped_vertices[1],
    clipped_vertices[2], clipped_points[0], clipped_points[1],
    clipped_points[2], width, height);
  if(clipped_count == 4) {
    render(frame_buffer, depth_buffer, clipped_vertices[0], clipped_vertices[2],
      clipped_vertices[3], clipped_points[0], clipped_points[2],
      clipped_points[3], width, height);
  }
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices,
    const std::vector<MeshTriangle>& triangles, const Matrix& transformation,
    int width, int height) {
  for(auto& triangle : triangles) {
    render(frame_buffer, depth_buffer, camera, vertices, triangle,
      transformation, width, height);
  }
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices, const MeshNode& node,
    const Transformation& transformation, const Matrix& parent_transformation,
    int width, int height) {
  auto next_transformation =
    parent_transformation * transformation.get_transformation(node);
  if(node.get_type() == MeshNode::Type::CHUNK) {
    for(auto& child : node.as_chunk()) {
      render(frame_buffer, depth_buffer, camera, vertices, child,
        transformation, next_transformation, width, height);
    }
  } else {
    render(frame_buffer, depth_buffer, camera, vertices, node.as_triangles(),
      next_transformation, width, height);
  }
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Camera& camera, const Mesh& mesh,
    const Transformation& transformation, int width, int height) {
  render(frame_buffer, depth_buffer, camera, mesh.m_vertices, mesh.m_root,
    transformation, Matrix::IDENTITY(), width, height);
}

void render(std::vector<std::uint32_t>& frame_buffer,
    std::vector<float>& depth_buffer, const Camera& camera,
    const SceneElement& element, int width, int height) {
  render(frame_buffer, depth_buffer, camera, element.get_mesh(),
    element.get_transformation(), width, height);
}

Mesh make_cube(Color color) {
  auto vertices = std::vector<Vertex>();
  vertices.reserve(24);
  auto triangles = std::vector<MeshTriangle>();
  triangles.reserve(12);
  auto addVertex = [&] (float x, float y, float z,
      float nx, float ny, float nz, Color c) {
    vertices.push_back(Vertex(Point(x, y, z), Vector(nx, ny, nz), c));
  };
  addVertex(1, -1, -1, 1, 0, 0, color);
  addVertex(1, 1, -1, 1, 0, 0, color);
  addVertex(1, 1, 1, 1, 0, 0, color);
  addVertex(1, -1, 1, 1, 0, 0, color);
  addVertex(-1, -1, 1, -1, 0, 0, color);
  addVertex(-1, 1, 1, -1, 0, 0, color);
  addVertex(-1, 1, -1, -1, 0, 0, color);
  addVertex(-1, -1, -1, -1, 0, 0, color);
  addVertex(-1, 1, -1, 0, 1, 0, color);
  addVertex(-1, 1, 1, 0, 1, 0, color);
  addVertex(1, 1, 1, 0, 1, 0, color);
  addVertex(1, 1, -1, 0, 1, 0, color);
  addVertex(-1, -1, 1, 0,-1, 0, color);
  addVertex(1, -1, 1, 0,-1, 0, color);
  addVertex(1, -1, -1, 0,-1, 0, color);
  addVertex(-1, -1, -1, 0,-1, 0, color);
  addVertex(-1, -1, 1, 0, 0, 1, color);
  addVertex(1, -1, 1, 0, 0, 1, color);
  addVertex(1, 1, 1, 0, 0, 1, color);
  addVertex(-1, 1, 1, 0, 0, 1, color);
  addVertex(1, -1, -1, 0, 0,-1, color);
  addVertex(-1, -1, -1, 0, 0,-1, color);
  addVertex(-1, 1, -1, 0, 0,-1, color);
  addVertex(1, 1, -1, 0, 0,-1, color);
  triangles.push_back({0, 1, 2});
  triangles.push_back({0, 2, 3});
  triangles.push_back({4, 5, 6});
  triangles.push_back({4, 6, 7});
  triangles.push_back({8, 9, 10});
  triangles.push_back({8, 10, 11});
  triangles.push_back({12, 14, 13});
  triangles.push_back({12, 15, 14});
  triangles.push_back({16, 17, 18});
  triangles.push_back({16, 18, 19});
  triangles.push_back({20, 21, 22});
  triangles.push_back({20, 22, 23});
  return Mesh(std::move(vertices), MeshNode(std::move(triangles)));
}

std::vector<std::unique_ptr<SceneElement>> make_cube_scene() {
  auto elements = std::vector<std::unique_ptr<SceneElement>>();
  elements.push_back(
    std::make_unique<SceneElement>(make_cube(Color(0, 0, 255, 255))));
  return elements;
}

std::vector<std::vector<int>> level_map = {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,1,0,1,1,0,1},
  {1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1},
};

std::vector<std::unique_ptr<SceneElement>> make_scene(
    const std::vector<std::vector<int>>& scene) {
  auto elements = std::vector<std::unique_ptr<SceneElement>>();
  auto depth = int(scene.size());
  for(auto y = 0; y < depth; ++y) {
    for(auto x = 0; x < int(scene[y].size()); ++x) {
      if(scene[y][x] == 1) {
        auto element =
          std::make_unique<SceneElement>(make_cube(Color(0, 0, 255, 255)));
        element->get_transformation().apply(
          translate(Vector(2 * x, 1, -2 * (depth - y))),
          element->get_mesh().m_root);
        elements.push_back(std::move(element));
      }
    }
  }
  return elements;
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
  std::freopen("stdout.log", "w", stdout);
  std::freopen("stderr.log", "w", stderr);
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  TTF_Init();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  const auto WIDTH = 640;
  const auto HEIGHT = 480;
  auto window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(!window) {
    std::cout << "Error creating window: " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetWindowGrab(window, SDL_TRUE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
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
  compute::context m_context(compute::opengl_create_shared_context());
  glViewport(0, 0, WIDTH, HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, WIDTH, HEIGHT, 0.0, 1.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  auto texture_id = make_shader(WIDTH, HEIGHT);
  auto texture =
    compute::opengl_texture(m_context, GL_TEXTURE_2D, 0, texture_id,
      compute::opengl_texture::mem_flags::write_only);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.f, 0.f, 0.f);
  auto frame_buffer = std::vector<std::uint32_t>(WIDTH * HEIGHT, 0);
  auto depth_buffer =
    std::vector<float>(WIDTH * HEIGHT, std::numeric_limits<float>::infinity());
  auto depth = int(level_map.size());
  auto cx = 3;
  auto cy = 2;
  auto camera = Camera(
    Point(2 * cx, 1, -2 * (depth - cy)), Vector(0, 0, -1), Vector(0, 1, 0));
//  auto scene = make_cube_scene();
  auto scene = make_scene(level_map);
  auto is_running = true;
  auto event = SDL_Event();
  auto window_id = SDL_GetWindowID(window);
  auto frame_count = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto fps = 0.f;
  while(is_running) {
    ++frame_count;
    std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
    std::fill(depth_buffer.begin(), depth_buffer.end(),
      std::numeric_limits<float>::infinity());
    glClear(GL_COLOR_BUFFER_BIT);
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_WINDOWEVENT && event.window.windowID == window_id &&
          event.window.event == SDL_WINDOWEVENT_CLOSE) {
        event.type = SDL_QUIT;
        SDL_PushEvent(&event);
      } else if(event.type == SDL_QUIT) {
        is_running = false;
      }
    }
    auto state = SDL_GetKeyboardState(nullptr);
    if(state[SDL_SCANCODE_ESCAPE]) {
      is_running = false;
    }
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
    int relX = 0, relY = 0;
    SDL_GetRelativeMouseState(&relX, &relY);
    float deltaAngle = relX * 0.0025f; 
    tilt(camera, deltaAngle, 0);
    for(auto& element : scene) {
      render(frame_buffer, depth_buffer, camera, *element, WIDTH, HEIGHT);
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA,
      GL_UNSIGNED_BYTE, frame_buffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(static_cast<float>(WIDTH), 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT));
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, static_cast<float>(HEIGHT));
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<float>(now - start_time).count();
    if(elapsed >= 1.f) {
      fps = frame_count / elapsed;
      frame_count = 0;
      start_time = now;
    }
    auto info = "Position: " +
      lexical_cast<std::string>(camera.get_position()) + "   ";
    info += "Direction: " + lexical_cast<std::string>(camera.get_direction()) +
      "   ";
    info += "Orientation: " +
      lexical_cast<std::string>(camera.get_orientation()) + "   ";
    info += "FPS: " + lexical_cast<std::string>(fps);
    draw_text(info, 12, 0, 0, SDL_Color{.g=255, .a=255});
    SDL_GL_SwapWindow(window);
  }
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  glDeleteTextures(1, &texture_id);
  SDL_Quit();
  return 0;
}
