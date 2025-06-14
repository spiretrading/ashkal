#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include <SDL.h>
#include <Windows.h>
#include "Ashkal/Camera.hpp"
#include "Ashkal/MeshLoader.hpp"
#include "Ashkal/Raster.hpp"
#include "Ashkal/Scene.hpp"
#include "Ashkal/SdlSurfaceColorSampler.hpp"
#include "Ashkal/ShadingSample.hpp"
#include "Ashkal/SolidColorSampler.hpp"
#include "Ashkal/TextRenderer.hpp"
#include "Version.hpp"

using namespace Ashkal;

std::pair<int, int> project_to_screen(
    const Point& point, int width, int height) {
  const auto THRESHOLD = 1e-5f;
  auto camera = Point(point.m_x, point.m_y, point.m_z + Camera::NEAR_PLANE_Z);
  if(camera.m_z >= 0) {
    camera.m_z = -THRESHOLD;
  }
  auto perspective = 1 / -camera.m_z;
  auto normalized_x = (height * camera.m_x * perspective) / width;
  auto normalized_y = camera.m_y * perspective;
  auto fx = (normalized_x + 1) * 0.5f * width;
  auto fy = (1 - (normalized_y + 1) * 0.5f) * height;
  return std::pair(int(fx), int(fy));
}

float compute_edge(const std::pair<int, int>& p1,
    const std::pair<int, int>& p2, const std::pair<float, float>& p) {
  return (p2.first - p1.first) * (p.second - p1.second) -
    (p2.second - p1.second) * (p.first - p1.first);
}

struct ShadedVertex {
  Point m_position;
  TextureCoordinate m_uv;
  ShadingTerm m_shading;
};

void render(const ShadedVertex& a, const ShadedVertex& b, const ShadedVertex& c,
    const Material& material, FrameBuffer& frame_buffer,
    DepthBuffer& depth_buffer) {
  auto screen_a = project_to_screen(
    a.m_position, frame_buffer.get_width(), frame_buffer.get_height());
  auto screen_b = project_to_screen(
    b.m_position, frame_buffer.get_width(), frame_buffer.get_height());
  auto screen_c = project_to_screen(
    c.m_position, frame_buffer.get_width(), frame_buffer.get_height());
  auto min_x =
    std::max(0, std::min({screen_a.first, screen_b.first, screen_c.first}));
  auto max_x = std::min(frame_buffer.get_width() - 1,
    std::max({screen_a.first, screen_b.first, screen_c.first}));
  auto min_y =
    std::max(0, std::min({screen_a.second, screen_b.second, screen_c.second}));
  auto max_y = std::min(frame_buffer.get_height() - 1,
    std::max({screen_a.second, screen_b.second, screen_c.second}));
  auto inv_z_a = -1 / (a.m_position.m_z - 1);
  auto inv_z_b = -1 / (b.m_position.m_z - 1);
  auto inv_z_c = -1 / (c.m_position.m_z - 1);
  auto uoz_a = a.m_uv.m_u * inv_z_a;
  auto uoz_b = b.m_uv.m_u * inv_z_b;
  auto uoz_c = c.m_uv.m_u * inv_z_c;
  auto voz_a = a.m_uv.m_v * inv_z_a;
  auto voz_b = b.m_uv.m_v * inv_z_b;
  auto voz_c = c.m_uv.m_v * inv_z_c;
  for(auto y = min_y; y <= max_y; ++y) {
    for(auto x = min_x; x <= max_x; ++x) {
      auto point = std::pair(x + 0.5f, y + 0.5f);
      auto w0 = compute_edge(screen_b, screen_c, point);
      auto w1 = compute_edge(screen_c, screen_a, point);
      auto w2 = compute_edge(screen_a, screen_b, point);
      if(w0 >= 0 && w1 >= 0 && w2 >= 0) {
        auto alpha = w0 / (w0 + w1 + w2);
        auto beta  = w1 / (w0 + w1 + w2);
        auto gamma = w2 / (w0 + w1 + w2);
        auto inv_z = alpha * inv_z_a + beta * inv_z_b + gamma * inv_z_c;
        auto depth = 1 / inv_z;
        if(depth < depth_buffer(x, y)) {
          depth_buffer(x, y) = depth;
          auto uv = TextureCoordinate(
            (alpha * uoz_a + beta * uoz_b + gamma * uoz_c) / inv_z,
            (alpha * voz_a + beta * voz_b + gamma * voz_c) / inv_z);
          auto texel = material.get_diffuseness().sample(uv);
          auto light_color = Color(
            static_cast<std::uint8_t>(alpha * a.m_shading.m_color.get_red() +
              beta * b.m_shading.m_color.get_red() +
              gamma * c.m_shading.m_color.get_red()),
            static_cast<std::uint8_t>(alpha * a.m_shading.m_color.get_green() +
              beta * b.m_shading.m_color.get_green() +
              gamma * c.m_shading.m_color.get_green()),
            static_cast<std::uint8_t>(alpha * a.m_shading.m_color.get_blue() +
              beta * b.m_shading.m_color.get_blue() +
              gamma * c.m_shading.m_color.get_blue()));
          auto intensity = alpha * a.m_shading.m_intensity +
            beta * b.m_shading.m_intensity + gamma * c.m_shading.m_intensity;
          auto shading = ShadingTerm(light_color, intensity);
          auto color = apply(shading, texel);
          frame_buffer(x, y) = apply(shading, texel);
        }
      }
    }
  }
}

float compute_interpolation_parameter(const Point& a, const Point& b) {
  return (a.m_z + Camera::NEAR_PLANE_Z) / (a.m_z - b.m_z);
}

Point intersect_near_plane(const Point& a, const Point& b, float t) {
  const auto THRESHOLD = 1e-5f;
  return Point(a.m_x + t * (b.m_x - a.m_x), a.m_y + t * (b.m_y - a.m_y),
    -Camera::NEAR_PLANE_Z - THRESHOLD);
}

void process_edge(const ShadedVertex& a, const ShadedVertex& b,
    ShadedVertex& split_vertex,
    std::array<const ShadedVertex*, 4>& clipped_vertices, int& n) {
  auto in0 = is_in_front(a.m_position);
  auto in1 = is_in_front(b.m_position);
  if(in0 && in1) {
    clipped_vertices[n] = &b;
    ++n;
  } else if(in0 && !in1) {
    auto t = compute_interpolation_parameter(a.m_position, b.m_position);
    auto p = intersect_near_plane(a.m_position, b.m_position, t);
    split_vertex.m_position = p;
    split_vertex.m_uv = TextureCoordinate(std::lerp(a.m_uv.m_u, b.m_uv.m_u, t),
      std::lerp(a.m_uv.m_v, b.m_uv.m_v, t));
    split_vertex.m_shading = b.m_shading;
    clipped_vertices[n] = &split_vertex;
    ++n;
  } else if(!in0 && in1) {
    auto t = compute_interpolation_parameter(a.m_position, b.m_position);
    auto p = intersect_near_plane(a.m_position, b.m_position, t);
    split_vertex.m_position = p;
    split_vertex.m_uv = TextureCoordinate(std::lerp(a.m_uv.m_u, b.m_uv.m_u, t),
      std::lerp(a.m_uv.m_v, b.m_uv.m_v, t));
    split_vertex.m_shading = b.m_shading;
    clipped_vertices[n] = &split_vertex;
    ++n;
    clipped_vertices[n] = &b;
    ++n;
  }
}

int clip(const ShadedVertex& a, const ShadedVertex& b, const ShadedVertex& c,
    std::array<ShadedVertex, 3>& split_vertices,
    std::array<const ShadedVertex*, 4>& clipped_vertices) {
  auto n = 0;
  process_edge(a, b, split_vertices[0], clipped_vertices, n);
  process_edge(b, c, split_vertices[1], clipped_vertices, n);
  process_edge(c, a, split_vertices[2], clipped_vertices, n);
  return n;
}

Vector transform_normal(const Matrix& transformation, const Vector& normal) {
  auto transformed_normal = Vector();
  transformed_normal.m_x = transformation.get(0, 0) * normal.m_x +
    transformation.get(1, 0) * normal.m_y +
    transformation.get(2, 0) * normal.m_z;
  transformed_normal.m_y = transformation.get(0, 1) * normal.m_x +
    transformation.get(1, 1) * normal.m_y +
    transformation.get(2, 1) * normal.m_z;
  transformed_normal.m_z = transformation.get(0, 2) * normal.m_x +
    transformation.get(1, 2) * normal.m_y +
    transformation.get(2, 2) * normal.m_z;
  return normalize(transformed_normal);
}

void render(const Model& model, const Fragment& fragment,
    const VertexTriangle& triangle, const Scene& scene, const Camera& camera,
    const Matrix& transformation, FrameBuffer& frame_buffer,
    DepthBuffer& depth_buffer) {
  auto& vertices = model.get_mesh().m_vertices;
  auto& a = vertices[triangle.m_a];
  auto shaded_a =
    ShadedVertex(world_to_view(transformation * a.m_position, camera), a.m_uv,
      calculate_shading(scene.get_ambient_light()) +
        calculate_shading(scene.get_directional_light(),
          transform_normal(transformation, a.m_normal)));
  auto& b = vertices[triangle.m_b];
  auto shaded_b =
    ShadedVertex(world_to_view(transformation * b.m_position, camera), b.m_uv,
      calculate_shading(scene.get_ambient_light()) +
        calculate_shading(scene.get_directional_light(),
          transform_normal(transformation, b.m_normal)));
  auto& c = vertices[triangle.m_c];
  auto shaded_c =
    ShadedVertex(world_to_view(transformation * c.m_position, camera), c.m_uv,
      calculate_shading(scene.get_ambient_light()) +
        calculate_shading(scene.get_directional_light(),
          transform_normal(transformation, c.m_normal)));
  if(is_in_front(shaded_a.m_position) && is_in_front(shaded_b.m_position) &&
      is_in_front(shaded_c.m_position)) {
    render(shaded_a, shaded_b, shaded_c, fragment.get_material(), frame_buffer,
      depth_buffer);
    return;
  }
  auto clipped_vertices = std::array<const ShadedVertex*, 4>();
  auto split_vertices = std::array<ShadedVertex, 3>();
  auto clipped_count =
    clip(shaded_a, shaded_b, shaded_c, split_vertices, clipped_vertices);
  if(clipped_count < 3) {
    return;
  }
  render(*clipped_vertices[0], *clipped_vertices[1], *clipped_vertices[2],
    fragment.get_material(), frame_buffer, depth_buffer);
  if(clipped_count == 4) {
    render(*clipped_vertices[0], *clipped_vertices[2], *clipped_vertices[3],
      fragment.get_material(), frame_buffer, depth_buffer);
  }
}

void render(const Model& model, const Fragment& fragment,
    const Scene& scene, const Camera& camera, const Matrix& transformation,
    FrameBuffer& frame_buffer, DepthBuffer& depth_buffer) {
  for(auto& triangle : fragment.get_triangles()) {
    render(model, fragment, triangle, scene, camera, transformation,
      frame_buffer, depth_buffer);
  }
}

void render(const Model& model, const MeshNode& node, const Scene& scene,
    const Camera& camera, const Matrix& parent_transformation,
    FrameBuffer& frame_buffer, DepthBuffer& depth_buffer) {
  auto next_transformation =
    parent_transformation * model.get_transformation().get_transformation(node);
  if(node.get_type() == MeshNode::Type::CHUNK) {
    for(auto& child : node.as_chunk()) {
      render(model, child, scene, camera, next_transformation, frame_buffer,
        depth_buffer);
    }
  } else {
    render(model, node.as_fragment(), scene, camera, next_transformation,
      frame_buffer, depth_buffer);
  }
}

void render(const Model& model, const Scene& scene, const Camera& camera,
    FrameBuffer& frame_buffer, DepthBuffer& depth_buffer) {
  render(model, model.get_mesh().m_root, scene, camera, Matrix::IDENTITY(),
    frame_buffer, depth_buffer);
}

void render(const Scene& scene, const Camera& camera, FrameBuffer& frame_buffer,
    DepthBuffer& depth_buffer) {
  for(auto i = 0; i != scene.get_model_count(); ++i) {
    render(scene.get_model(i), scene, camera, frame_buffer, depth_buffer);
  }
}

Mesh make_cube(std::shared_ptr<ColorSampler> texture) {
  auto vertices = std::vector<Vertex>();
  vertices.reserve(24);
  auto triangles = std::vector<VertexTriangle>();
  triangles.reserve(12);
  vertices.emplace_back(
    Point(1, -1, -1), TextureCoordinate(1, 0), Vector(1, 0, 0));
  vertices.emplace_back(
    Point(1, 1, -1), TextureCoordinate(1, 1), Vector(1, 0, 0));
  vertices.emplace_back(
    Point(1, 1, 1), TextureCoordinate(0, 1), Vector(1, 0, 0));
  vertices.emplace_back(
    Point(1, -1, 1), TextureCoordinate(0, 0), Vector(1, 0, 0));
  vertices.emplace_back(
    Point(-1, -1, 1), TextureCoordinate(1, 0), Vector(-1, 0, 0));
  vertices.emplace_back(
    Point(-1, 1, 1), TextureCoordinate(1, 1), Vector(-1, 0, 0));
  vertices.emplace_back(
    Point(-1, 1, -1), TextureCoordinate(0, 1), Vector(-1, 0, 0));
  vertices.emplace_back(
    Point(-1, -1, -1), TextureCoordinate(0, 0), Vector(-1, 0, 0));
  vertices.emplace_back(
    Point(-1, 1, -1), TextureCoordinate(0, 1), Vector(0, 1, 0));
  vertices.emplace_back(
    Point(-1, 1, 1), TextureCoordinate(0, 0), Vector(0, 1, 0));
  vertices.emplace_back(
    Point(1, 1, 1), TextureCoordinate(1, 0), Vector(0, 1, 0));
  vertices.emplace_back(
    Point(1, 1, -1), TextureCoordinate(1, 1), Vector(0, 1, 0));
  vertices.emplace_back(
    Point(-1, -1, 1), TextureCoordinate(0, 0), Vector(0, -1, 0));
  vertices.emplace_back(
    Point(1, -1, 1), TextureCoordinate(1, 0), Vector(0, -1, 0));
  vertices.emplace_back(
    Point(1, -1, -1), TextureCoordinate(1, 1), Vector(0, -1, 0));
  vertices.emplace_back(
    Point(-1, -1, -1), TextureCoordinate(0, 1), Vector(0, -1, 0));
  vertices.emplace_back(
    Point(-1, -1, 1), TextureCoordinate(0, 0), Vector(0, 0, 1));
  vertices.emplace_back(
    Point(1, -1, 1), TextureCoordinate(1, 0), Vector(0, 0, 1));
  vertices.emplace_back(
    Point(1, 1, 1), TextureCoordinate(1, 1), Vector(0, 0, 1));
  vertices.emplace_back(
    Point(-1, 1, 1), TextureCoordinate(0, 1), Vector(0, 0, 1));
  vertices.emplace_back(
    Point(1, -1, -1), TextureCoordinate(0, 0), Vector(0, 0, -1));
  vertices.emplace_back(
    Point(-1, -1, -1), TextureCoordinate(1, 0), Vector(0, 0, -1));
  vertices.emplace_back(
    Point(-1, 1, -1), TextureCoordinate(1, 1), Vector(0, 0, -1));
  vertices.emplace_back(
    Point(1, 1, -1), TextureCoordinate(0, 1), Vector(0, 0, -1));
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
  auto material = std::make_shared<Material>(std::move(texture));
  auto fragment = Fragment(std::move(triangles), std::move(material));
  return Mesh(std::move(vertices), MeshNode(std::move(fragment)));
}

auto level_map = std::vector<std::vector<int>> {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,1,0,1,1,0,1},
  {1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1},
};

std::unique_ptr<Scene> make_scene(const std::vector<std::vector<int>>& map) {
  auto scene = std::make_unique<Scene>();
  scene->set(AmbientLight(Color(255, 255, 255, 255), .7));
  scene->set(DirectionalLight(
    normalize(Vector(.2, -1, 0.3)), Color(255, 255, 240, 255), .8));
  auto depth = int(map.size());
  auto wall_texture = load_sampler("texture1.bmp");
  for(auto y = 0; y < depth; ++y) {
    for(auto x = 0; x < int(map[y].size()); ++x) {
      if(map[y][x] == 1) {
        auto model = std::make_unique<Model>(make_cube(wall_texture));
        model->get_transformation().apply(
          translate(Vector(2 * x, 1, -2 * (depth - y))),
          model->get_mesh().m_root);
        scene->add(std::move(model));
      }
    }
  }
  auto ceiling_texture =
    std::make_shared<SolidColorSampler>(Color(178, 34, 34, 255));
  auto ceiling = std::make_unique<Model>(make_cube(ceiling_texture));
  ceiling->get_transformation().apply(
    scale_y(.001), ceiling->get_mesh().m_root);
  ceiling->get_transformation().apply(scale_x(8), ceiling->get_mesh().m_root);
  ceiling->get_transformation().apply(scale_z(5), ceiling->get_mesh().m_root);
  ceiling->get_transformation().apply(translate(Vector(7, 2, -6)),
    ceiling->get_mesh().m_root);
  scene->add(std::move(ceiling));
  auto floor_texture =
    std::make_shared<SolidColorSampler>(Color(116, 116, 116, 255));
  auto floor = std::make_unique<Model>(make_cube(floor_texture));
  floor->get_transformation().apply(scale_y(.001), floor->get_mesh().m_root);
  floor->get_transformation().apply(scale_x(8), floor->get_mesh().m_root);
  floor->get_transformation().apply(scale_z(5), floor->get_mesh().m_root);
  floor->get_transformation().apply(translate(Vector(7, -.1, -6)),
    floor->get_mesh().m_root);
  scene->add(std::move(floor));
  return scene;
}

std::unique_ptr<Scene> make_object_viewer(const std::filesystem::path& path) {
  auto scene = std::make_unique<Scene>();
  scene->set(AmbientLight(Color(255, 255, 255, 255), 2));
  scene->set(DirectionalLight(
    normalize(Vector(.2, -1, 0.3)), Color(255, 255, 240, 255), 2.5));
  auto mesh = load_mesh(path);
  auto model = std::make_unique<Model>(std::move(mesh));
  scene->add(std::move(model));
  return scene;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR pCmdLine, int nCmdShow) {
  std::freopen("stdout.log", "w", stdout);
  std::freopen("stderr.log", "w", stderr);
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
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
  auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if(!renderer) {
    std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
    return 1;
  }
  auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if(!texture) {
    std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetWindowGrab(window, SDL_TRUE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  auto text_renderer = TextRenderer("C:\\Windows\\Fonts\\arial.ttf", 12);
  auto frame_buffer = FrameBuffer(WIDTH, HEIGHT);
  auto depth_buffer = DepthBuffer(WIDTH, HEIGHT);
  auto depth = int(level_map.size());
  auto cx = 3;
  auto cy = 2;
  auto camera = Camera(
    Point(2 * cx, 1, -2 * (depth - cy)), Vector(0, 0, -1), Vector(0, 1, 0));
//  auto scene = make_scene(level_map);
  auto scene = make_object_viewer(std::filesystem::path(pCmdLine).string());
  auto is_running = true;
  auto event = SDL_Event();
  auto window_id = SDL_GetWindowID(window);
  auto frame_count = 0;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto fps = 0.f;
  while(is_running) {
    ++frame_count;
    frame_buffer.fill(Color(0));
    depth_buffer.fill(std::numeric_limits<float>::infinity());
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
    if(state[SDL_SCANCODE_SPACE]) {
      move_up(camera, 1 / 10.f);
    } else if(state[SDL_SCANCODE_LCTRL]) {
      move_down(camera, 1 / 10.f);
    }
    int relX = 0, relY = 0;
    SDL_GetRelativeMouseState(&relX, &relY);
    float deltaAngle = relX * 0.0025f;
    tilt(camera, deltaAngle, 0);
    render(*scene, camera, frame_buffer, depth_buffer);
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<float>(now - start_time).count();
    if(elapsed >= 1.f) {
      fps = frame_count / elapsed;
      frame_count = 0;
      start_time = now;
    }
    auto ss = std::stringstream();
    ss << "Position: " << camera.get_position() << "\n" <<
      "Direction: " << camera.get_direction() << "\n" <<
      "Orientation: " << camera.get_orientation() << "\n" <<
      "FPS: " << fps;
    text_renderer.render(ss.str(), 0, 0, Color(0, 255, 0, 255), frame_buffer);
    SDL_UpdateTexture(
      texture, nullptr, frame_buffer.data(), WIDTH * sizeof(std::uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
