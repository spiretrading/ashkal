#include <iostream>
#include <limits>
#include <numbers>
#include <utility>
#include <vector>
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
  if(point.m_z >= 0) {
    return std::pair(-1, -1);
  }
  auto perspective = 1.0f / -point.m_z;
  auto aspect_ratio = float(width) / float(height);
  auto x = (point.m_x * perspective) / aspect_ratio;
  auto y = point.m_y * perspective;
  auto screen_x = int((x + 1.0f) * 0.5f * width);
  auto screen_y = int((1.0f - (y + 1.0f) * 0.5f) * height);
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
    std::vector<float> depth_buffer, const Camera& camera,
    const std::vector<Vertex>& vertices, const MeshTriangle& triangle,
    const Matrix& transformation, int width, int height) {
  auto& a = vertices[triangle.m_a];
  auto& b = vertices[triangle.m_b];
  auto& c = vertices[triangle.m_c];
  auto camera_a = transform(transformation * a.m_position, camera);
  auto camera_b = transform(transformation * b.m_position, camera);
  auto camera_c = transform(transformation * c.m_position, camera);
  auto screen_a = project_to_screen(camera_a, width, height);
  auto screen_b = project_to_screen(camera_b, width, height);
  auto screen_c = project_to_screen(camera_c, width, height);
  if(screen_a.first < 0 || screen_b.first < 0 || screen_c.first < 0) {
    return;
  }
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
            (std::uint32_t(final_color.m_alpha) << 24) |
            (std::uint32_t(final_color.m_red)   << 16) |
            (std::uint32_t(final_color.m_green) << 8 ) |
            (std::uint32_t(final_color.m_blue));
          frame_buffer[index] = pixel;
        }
      }
    }
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
  addVertex(1, -1, -1, 1, 0, 0, white);
  addVertex(1, 1, -1, 1, 0, 0, red);
  addVertex(1, 1, 1, 1, 0, 0, green);
  addVertex(1, -1, 1, 1, 0, 0, blue);
  addVertex(-1, -1, 1, -1, 0, 0, blue);
  addVertex(-1, 1, 1, -1, 0, 0, red);
  addVertex(-1, 1, -1, -1, 0, 0, green);
  addVertex(-1, -1, -1, -1, 0, 0, white);
  addVertex(-1, 1, -1, 0, 1, 0, red);
  addVertex(-1, 1, 1, 0, 1, 0, green);
  addVertex(1, 1, 1, 0, 1, 0, white);
  addVertex(1, 1, -1, 0, 1, 0, blue);
  addVertex(-1, -1, 1, 0,-1, 0, red);
  addVertex(1, -1, 1, 0,-1, 0, green);
  addVertex(1, -1, -1, 0,-1, 0, blue);
  addVertex(-1, -1, -1, 0,-1, 0, white);
  addVertex(-1, -1, 1, 0, 0, 1, blue);
  addVertex(1, -1, 1, 0, 0, 1, green);
  addVertex(1, 1, 1, 0, 0, 1, white);
  addVertex(-1, 1, 1, 0, 0, 1, red);
  addVertex(1, -1, -1, 0, 0,-1, blue);
  addVertex(-1, -1, -1, 0, 0,-1, red);
  addVertex(-1, 1, -1, 0, 0,-1, green);
  addVertex(1, 1, -1, 0, 0,-1, white);
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
  const auto WIDTH = 1920;
  const auto HEIGHT = 1080;
  auto window = SDL_CreateWindow("Software 3D Rasterizer",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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
  auto camera = Camera(Point(0, 0, 10), Vector(0, 0, -1), Vector(0, 1, 0));
  auto cube = SceneElement(make_cube());
  auto is_running = true;
  auto event = SDL_Event();
  auto window_id = SDL_GetWindowID(window);
  auto frames = 0;
  auto start = std::chrono::high_resolution_clock::now();
  auto last_mouse_x = std::numeric_limits<int>::min();
  auto last_mouse_y = std::numeric_limits<int>::min();
  while(is_running) {
    ++frames;
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
    cube.get_transformation().apply(roll(0.1), cube.get_mesh().m_root);
    render(frame_buffer, depth_buffer, camera, cube, WIDTH, HEIGHT);
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
    auto info = "Position: " +
      lexical_cast<std::string>(camera.get_position()) + "\n";
    info += "Mouse: (" + lexical_cast<std::string>(mouse_x) + ", " +
      lexical_cast<std::string>(mouse_y) + ")\n";
    info += "Direction: " + lexical_cast<std::string>(camera.get_direction()) +
      "\n";
    info += "Orientation: " +
      lexical_cast<std::string>(camera.get_orientation()) + "\n";
    info += "FPS: " + lexical_cast<std::string>(
      frames / std::chrono::duration_cast<std::chrono::duration<double>>(
      std::chrono::high_resolution_clock::now() - start).count());
    draw_text(info, 12, 0, 0, SDL_Color{.g=255, .a=255});
    SDL_GL_SwapWindow(window);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << (
    frames / std::chrono::duration_cast<std::chrono::duration<double>>(
      end - start).count()) << std::endl;
  SDL_DestroyWindow(window);
  SDL_GL_DeleteContext(gl_context);
  glDeleteTextures(1, &texture_id);
  SDL_Quit();
  return 0;
}
