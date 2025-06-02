#include <iostream>
#include <limits>
#include <numbers>
#include <utility>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Ashkal/Camera.hpp"
#include "Ashkal/SceneElement.hpp"
#include "Version.hpp"

using namespace Ashkal;

Point transform(const Point& point, const Camera& camera) {
  auto rel = point - camera.get_position();
  return Point(dot(rel, camera.get_right()),
    dot(rel, camera.get_orientation()), dot(rel, -camera.get_direction()));
}

std::pair<int, int> project_to_screen(
    const Point& point, int width, int height) {
  if(point.m_z >= 0) {
    return {-1, -1};
  }
  auto x = point.m_x / -point.m_z;
  auto y = point.m_y / -point.m_z;
  auto px = int((x + 1) * 0.5f * width);
  auto py = int((1 - (y + 1) * 0.5f) * height);
  return std::pair(px, py);
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

void render(std::vector<std::uint32_t>& frame_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices, const MeshTriangle& triangle,
    const Matrix& transformation, int width, int height) {
  auto& a = vertices[triangle.m_a];
  auto& b = vertices[triangle.m_b];
  auto& c = vertices[triangle.m_c];
  auto a_position = transform(transformation * a.m_position, camera);
  auto b_position = transform(transformation * b.m_position, camera);
  auto c_position = transform(transformation * c.m_position, camera);
  auto a_screen = project_to_screen(a_position, width, height);
  auto b_screen = project_to_screen(b_position, width, height);
  auto c_screen = project_to_screen(c_position, width, height);
  if(a_screen.first < 0 || b_screen.first < 0 || c_screen.first < 0) {
    return;
  }
  auto min_x =
    std::max(0, std::min({a_screen.first, b_screen.first, c_screen.first}));
  auto max_x = std::min(
    width - 1, std::max({a_screen.first, b_screen.first, c_screen.first}));
  auto min_y =
    std::max(0, std::min({a_screen.second, b_screen.second, c_screen.second}));
  auto max_y = std::min(
    height - 1, std::max({a_screen.second, b_screen.second, c_screen.second}));
  for(auto y = min_y; y <= max_y; ++y) {
    for(auto x = min_x; x <= max_x; ++x) {
      auto p = std::pair(x, y);
      auto w0 = compute_edge(b_screen, c_screen, p);
      auto w1 = compute_edge(c_screen, a_screen, p);
      auto w2 = compute_edge(a_screen, b_screen, p);
      if(w0 >= 0 && w1 >= 0 && w2 >= 0) {
        auto c01 = lerp(a.m_color, b.m_color, w1 / (w0 + w1 + w2));
        auto finalC = lerp(c01, c.m_color, w2 / (w0 + w1 + w2));
        auto pixel = (std::uint32_t(finalC.m_alpha) << 24) |
                          (std::uint32_t(finalC.m_red)   << 16) |
                          (std::uint32_t(finalC.m_green) << 8 ) |
                          (std::uint32_t(finalC.m_blue));
        frame_buffer[y * width + x] = pixel;
      }
    }
  }
}

void render(std::vector<std::uint32_t>& frame_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices,
    const std::vector<MeshTriangle>& triangles, const Matrix& transformation,
    int width, int height) {
  for(auto& triangle : triangles) {
    render(frame_buffer, camera, vertices, triangle, transformation, width,
      height);
  }
}

void render(std::vector<std::uint32_t>& frame_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices, const MeshNode& node,
    const Transformation& transformation, const Matrix& parent_transformation,
    int width, int height) {
  auto next_transformation =
    parent_transformation * transformation.get_transformation(node);
  if(node.get_type() == MeshNode::Type::CHUNK) {
    for(auto& child : node.as_chunk()) {
      render(frame_buffer, camera, vertices, child, transformation,
        next_transformation, width, height);
    }
  } else {
    render(frame_buffer, camera, vertices, node.as_triangles(),
      next_transformation, width, height);
  }
}

void render(std::vector<std::uint32_t>& frame_buffer, const Camera& camera,
    const Mesh& mesh, const Transformation& transformation, int width,
    int height) {
  render(frame_buffer, camera, mesh.m_vertices, mesh.m_root, transformation,
    Matrix::IDENTITY(), width, height);
}

void render(std::vector<std::uint32_t>& frame_buffer, const Camera& camera,
    const SceneElement& element, int width, int height) {
  render(frame_buffer, camera, element.get_mesh(), element.get_transformation(),
    width, height);
}

Mesh make_cube() {
  auto vertices = std::vector<Vertex>();
  vertices.reserve(24);
  auto triangles = std::vector<MeshTriangle>();
  triangles.reserve(12);
  auto addVertex = [&] (float x, float y, float z,
      float nx, float ny, float nz, Color c) {
    vertices.push_back(Vertex(Point(x, y, z), Vector(nx, ny, nz), c));
  };
  auto white = Color(255, 255, 255, 255);
  auto red = Color(255, 0, 0, 255);
  auto green = Color(0, 255, 0, 255);
  auto blue = Color(0, 0, 255, 255);

  // +X face (normal = {1,0,0})
  //  (1, -1, -1), (1, 1, -1), (1, 1, 1), (1, -1, 1)
  addVertex(1, -1, -1, 1, 0, 0, white); // idx  0
  addVertex(1, 1, -1, 1, 0, 0, red); // idx  1
  addVertex(1, 1, 1, 1, 0, 0, green); // idx  2
  addVertex(1, -1, 1, 1, 0, 0, blue); // idx  3

  // -X face (normal = {-1,0,0})
  //  (-1, -1,  1), (-1,  1,  1), (-1,  1, -1), (-1, -1, -1)
  addVertex(-1, -1, 1, -1, 0, 0, blue); // idx  4
  addVertex(-1, 1, 1, -1, 0, 0, red); // idx  5
  addVertex(-1, 1, -1, -1, 0, 0, green); // idx  6
  addVertex(-1, -1, -1, -1, 0, 0, white); // idx  7

  // +Y face (normal = {0,1,0})
  //  (-1, 1, -1), (1, 1, -1), (1, 1, 1), (-1, 1, 1)
  addVertex(-1, 1, -1, 0, 1, 0, red); // idx  8
  addVertex( 1, 1, -1, 0, 1, 0, blue); // idx  9
  addVertex( 1, 1, 1, 0, 1, 0, white); // idx 10
  addVertex(-1, 1, 1, 0, 1, 0, green); // idx 11

  // -Y face (normal = {0,-1,0})
  //  (-1, -1,  1), (1, -1,  1), (1, -1, -1), (-1, -1, -1)
  addVertex(-1, -1, 1, 0,-1, 0, red); // idx 12
  addVertex( 1, -1, 1, 0,-1, 0, green); // idx 13
  addVertex( 1, -1, -1, 0,-1, 0, blue); // idx 14
  addVertex(-1, -1, -1, 0,-1, 0, white); // idx 15

  // +Z face (normal = {0,0,1})
  //  (-1, -1, 1), (1, -1, 1), (1, 1, 1), (-1, 1, 1)
  addVertex(-1, -1, 1, 0, 0, 1, blue); // idx 16
  addVertex( 1, -1, 1, 0, 0, 1, green); // idx 17
  addVertex( 1,  1, 1, 0, 0, 1, white); // idx 18
  addVertex(-1,  1, 1, 0, 0, 1, red); // idx 19

  // -Z face (normal = {0,0,-1})
  //  (1, -1, -1), (-1, -1, -1), (-1, 1, -1), (1, 1, -1)
  addVertex( 1, -1, -1, 0, 0,-1, blue); // idx 20
  addVertex(-1, -1, -1, 0, 0,-1, red); // idx 21
  addVertex(-1,  1, -1, 0, 0,-1, green); // idx 22
  addVertex( 1,  1, -1, 0, 0,-1, white); // idx 23

    // Now add 12 triangles (two per face), using the indices above:

  // +X face: (0,1,2), (0,2,3)
  triangles.push_back({0, 1, 2});
  triangles.push_back({0, 2, 3});

  // -X face: (4,5,6), (4,6,7)
  triangles.push_back({4, 5, 6});
  triangles.push_back({4, 6, 7});

  // +Y face: ( 8,  9, 10), ( 8, 10, 11)
  triangles.push_back({8, 9, 10});
  triangles.push_back({8, 10, 11});

  // -Y face: (12, 13, 14), (12, 14, 15)
  triangles.push_back({12, 13, 14});
  triangles.push_back({12, 14, 15});

  // +Z face: (16, 17, 18), (16, 18, 19)
  triangles.push_back({16, 17, 18});
  triangles.push_back({16, 18, 19});

  // -Z face: (20, 21, 22), (20, 22, 23)
  triangles.push_back({20, 21, 22});
  triangles.push_back({20, 22, 23});

  return Mesh(std::move(vertices), MeshNode(std::move(triangles)));
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR pCmdLine, int nCmdShow) {
  std::freopen("stdout.log", "w", stdout);
  std::freopen("stderr.log", "w", stderr);
  const auto WIDTH = 1920;
  const auto HEIGHT = 1080;
  SDL_Init(SDL_INIT_VIDEO);
  auto window = SDL_CreateWindow("Software 3D Rasterizer",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  auto frame_buffer = std::vector<std::uint32_t>(WIDTH * HEIGHT, 0);
  auto camera = Camera(Point(0, 0, 10), Vector(0, 0, -1), Vector(0, 1, 0));
  auto cube = SceneElement(make_cube());
  auto is_running = true;
  auto event = SDL_Event();
  auto window_id = SDL_GetWindowID(window);
  auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  auto last_mouse_x = std::numeric_limits<int>::min();
  auto last_mouse_y = std::numeric_limits<int>::min();
  while(is_running) {
    std::fill(frame_buffer.begin(), frame_buffer.end(), 0x00000000);
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
          is_running = false;
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
      auto delta_x = ((mouse_x - last_mouse_x) / (WIDTH / 2.f)) *
        (std::numbers::pi_v<float> / 2);
      auto delta_y = ((mouse_y - last_mouse_y) / (HEIGHT / 2.f)) *
        (std::numbers::pi_v<float> / 2);
      tilt(camera, delta_x, delta_y);
    }
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
//    cube.get_transformation().apply(roll(0.1), cube.get_mesh().m_root);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    render(frame_buffer, camera, cube, WIDTH, HEIGHT);
    SDL_UpdateTexture(
      texture, nullptr, frame_buffer.data(), WIDTH * sizeof(std::uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    SDL_Delay(32);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
