#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include <SDL.h>
#include <Windows.h>
#include "Ashkal/Camera.hpp"
#include "Ashkal/MeshLoader.hpp"
#include "Ashkal/Raster.hpp"
#include "Ashkal/Renderer.hpp"
#include "Ashkal/Scene.hpp"
#include "Ashkal/SdlSurfaceColorSampler.hpp"
#include "Ashkal/ShadingSample.hpp"
#include "Ashkal/SolidColorSampler.hpp"
#include "Ashkal/TextRenderer.hpp"
#include "Version.hpp"

using namespace Ashkal;

float compute_edge(const ScreenCoordinate& p1,
    const ScreenCoordinate& p2, const FloatScreenCoordinate& p) {
  return (p2.m_x - p1.m_x) * (p.m_y - p1.m_y) -
    (p2.m_y - p1.m_y) * (p.m_x - p1.m_x);
}

struct ShadedVertex {
  Point m_position;
  TextureCoordinate m_uv;
  ShadingTerm m_shading;
};

void render(const ShadedVertex& a, const ShadedVertex& b, const ShadedVertex& c,
    const Material& material, const Camera& camera, FrameBuffer& frame_buffer,
    DepthBuffer& depth_buffer) {
  auto screen_a = project_to_screen(
    a.m_position, camera, frame_buffer.get_width(), frame_buffer.get_height());
  auto screen_b = project_to_screen(
    b.m_position, camera, frame_buffer.get_width(), frame_buffer.get_height());
  auto screen_c = project_to_screen(
    c.m_position, camera, frame_buffer.get_width(), frame_buffer.get_height());
  auto min_x =
    std::max(0, std::min({screen_a.m_x, screen_b.m_x, screen_c.m_x}));
  auto max_x = std::min(frame_buffer.get_width() - 1,
    std::max({screen_a.m_x, screen_b.m_x, screen_c.m_x}));
  auto min_y =
    std::max(0, std::min({screen_a.m_y, screen_b.m_y, screen_c.m_y}));
  auto max_y = std::min(frame_buffer.get_height() - 1,
    std::max({screen_a.m_y, screen_b.m_y, screen_c.m_y}));
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
      auto point = FloatScreenCoordinate(x + 0.5f, y + 0.5f);
      auto w0 = compute_edge(screen_b, screen_c, point);
      auto w1 = compute_edge(screen_c, screen_a, point);
      auto w2 = compute_edge(screen_a, screen_b, point);
      if(w0 >= 0 && w1 >= 0 && w2 >= 0) {
        auto alpha = w0 / (w0 + w1 + w2);
        auto beta  = w1 / (w0 + w1 + w2);
        auto gamma = w2 / (w0 + w1 + w2);
        auto inv_z = alpha * inv_z_a + beta * inv_z_b + gamma * inv_z_c;
        auto depth = 1 / inv_z;
        if(depth <= depth_buffer(x, y)) {
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

ShadedVertex intersect(
    const ShadedVertex& a, const ShadedVertex& b, const Plane& plane) {
  auto d_a = distance(plane, a.m_position);
  auto d_b = distance(plane, b.m_position);
  auto t = d_a / (d_a - d_b);
  auto result = a;
  result.m_position = a.m_position + t * (b.m_position - a.m_position);
  result.m_uv = TextureCoordinate(
    std::lerp(a.m_uv.m_u, b.m_uv.m_u, t), std::lerp(a.m_uv.m_v, b.m_uv.m_v, t));
  result.m_shading = ShadingTerm(
    lerp(a.m_shading.m_color, b.m_shading.m_color, t),
    std::lerp(a.m_shading.m_intensity, b.m_shading.m_intensity, t));
  return result;
}

std::vector<ShadedVertex> clip(const ShadedVertex& v0, const ShadedVertex& v1,
    const ShadedVertex& v2, const Plane& plane) {
  auto d0 = is_in_front(plane, v0.m_position);
  auto d1 = is_in_front(plane, v1.m_position);
  auto d2 = is_in_front(plane, v2.m_position);
  if(d0 && d1 && d2) {
    return std::vector{v0, v1, v2};
  } else if(!d0 && !d1 && !d2) {
    return {};
  }
  auto count = 0;
  if(d0) {
    ++count;
  }
  if(d1) {
    ++count;
  }
  if(d2) {
    ++count;
  }
  if(count == 1) {
    auto [a, b, c] = [&] {
      if(d0) {
        return std::tuple(&v0, &v1, &v2);
      } else if(d1) {
        return std::tuple(&v1, &v2, &v0);
      }
      return std::tuple(&v2, &v0, &v1);
    }();
    auto clipped_b = intersect(*a, *b, plane);
    auto clipped_c = intersect(*a, *c, plane);
    return std::vector{*a, clipped_b, clipped_c};
  } else {
    auto [c, a, b] = [&] {
      if(!d0) {
        return std::tuple(&v0, &v1, &v2);
      } else if(!d1) {
        return std::tuple(&v1, &v2, &v0);
      }
      return std::tuple(&v2, &v0, &v1);
    }();
    auto clipped_a = intersect(*a, *c, plane);
    auto clipped_b = intersect(*b, *c, plane);
    return std::vector{*a, *b, clipped_a, clipped_a, *b, clipped_b};
  }
}

Vector transform_normal(const Matrix& transformation, const Vector& normal) {
  auto transformed_normal = Vector();
  transformed_normal.m_x = transformation.get(0, 0) * normal.m_x +
    transformation.get(0, 1) * normal.m_y +
    transformation.get(0, 2) * normal.m_z;
  transformed_normal.m_y = transformation.get(1, 0) * normal.m_x +
    transformation.get(1, 1) * normal.m_y +
    transformation.get(1, 2) * normal.m_z;
  transformed_normal.m_z = transformation.get(2, 0) * normal.m_x +
    transformation.get(2, 1) * normal.m_y +
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
  auto f_x = camera.get_horizontal_focal_length();
  auto f_y = camera.get_focal_length();
  auto planes = std::array{
    Plane(Vector(0.f, 0.f, -1.f), Camera::NEAR_PLANE_Z),
    Plane(normalize(Vector(  f_x, 0.f, -1.f)), 0.f),
    Plane(normalize(Vector( -f_x, 0.f, -1.f)), 0.f),
    Plane(normalize(Vector( 0.f,  f_y, -1.f)), 0.f),
    Plane(normalize(Vector( 0.f, -f_y, -1.f)), 0.f)};
  auto clipped_vertices = std::vector{shaded_a, shaded_b, shaded_c};
  for(auto& plane : planes) {
    auto new_vertices = std::vector<ShadedVertex>();
    for(auto i = std::size_t(0); i + 2 < clipped_vertices.size(); i += 3) {
      auto segment = clip(clipped_vertices[i], clipped_vertices[i + 1],
        clipped_vertices[i + 2], plane);
      new_vertices.insert(new_vertices.end(), segment.begin(), segment.end());
    }
    clipped_vertices = std::move(new_vertices);
    if(clipped_vertices.empty()) {
      return;
    }
  }
  for(auto i = std::size_t(0); i + 2 < clipped_vertices.size(); i += 3) {
    render(clipped_vertices[i], clipped_vertices[i + 1],
      clipped_vertices[i + 2], fragment.get_material(), camera, frame_buffer,
      depth_buffer);
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
    parent_transformation * model.get_segment(node).get_transformation();
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
    auto& model = scene.get_model(i);
    auto& segment = model.get_segment(model.get_mesh().m_root);
    auto bounding_box = segment.get_bounding_box();
    bounding_box.apply(segment.get_transformation());
//    if(intersects(camera.get_frustum(), bounding_box)) {
      render(model, scene, camera, frame_buffer, depth_buffer);
//    }
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
  scene->set(AmbientLight(Color(255, 255, 255), .7));
  scene->set(
    DirectionalLight(normalize(Vector(.2, -1, 0.3)), Color(255, 255, 240), .8));
  auto depth = int(map.size());
//  auto wall_texture = load_sampler("texture1.bmp");
  auto wall_texture =
    std::make_shared<SolidColorSampler>(Color(255, 0, 0));
  for(auto y = 0; y < depth; ++y) {
    for(auto x = 0; x < int(map[y].size()); ++x) {
      if(map[y][x] == 1) {
        auto model = std::make_unique<Model>(make_cube(wall_texture));
        model->get_segment(model->get_mesh().m_root).apply(
          translate(Vector(2 * x, 1, -2 * (depth - y))));
        scene->add(std::move(model));
      }
    }
  }
  auto ceiling_texture =
    std::make_shared<SolidColorSampler>(Color(178, 34, 34));
  auto ceiling = std::make_unique<Model>(make_cube(ceiling_texture));
  auto& ceiling_segment = ceiling->get_segment(ceiling->get_mesh().m_root);
  ceiling_segment.apply(scale_y(.001));
  ceiling_segment.apply(scale_x(8));
  ceiling_segment.apply(scale_z(5));
  ceiling_segment.apply(translate(Vector(7, 2, -6)));
  scene->add(std::move(ceiling));
  auto floor_texture =
    std::make_shared<SolidColorSampler>(Color(116, 116, 116));
  auto floor = std::make_unique<Model>(make_cube(floor_texture));
  auto& floor_segment = floor->get_segment(floor->get_mesh().m_root);
  floor_segment.apply(scale_y(.001));
  floor_segment.apply(scale_x(8));
  floor_segment.apply(scale_z(5));
  floor_segment.apply(translate(Vector(7, -.1, -6)));
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

std::unique_ptr<Scene> make_simple_scene() {
  auto scene = std::make_unique<Scene>();
  scene->set(AmbientLight(Color(255, 255, 255, 255), 2));
  scene->set(DirectionalLight(
    normalize(Vector(.2, -1, 0.3)), Color(255, 255, 240, 255), 2.5));
  auto red_sampler = std::make_shared<SolidColorSampler>(Color(255, 0, 0, 255));
  auto cube_mesh = make_cube(red_sampler);
  auto cube_model = std::make_unique<Model>(std::move(cube_mesh));
  scene->add(std::move(cube_model));
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
  auto scene = make_simple_scene();
  auto camera = Camera(Point(0, 0, -5), Vector(0, 0, 1), Vector(0, 1, 0),
    WIDTH / float(HEIGHT), std::numbers::pi_v<float> / 2);
/*
  auto depth = int(level_map.size());
  auto cx = 3;
  auto cy = 2;
  auto aspect = WIDTH / static_cast<float>(HEIGHT);
  auto camera = Camera(Point(2 * cx, 1, -2 * (depth - cy)), Vector(0, 0, -1),
    Vector(0, 1, 0), WIDTH / static_cast<float>(HEIGHT));
  auto scene = make_scene(level_map);
//  auto scene = make_object_viewer(std::filesystem::path(pCmdLine).string());
*/
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
