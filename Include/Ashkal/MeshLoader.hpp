#ifndef ASHKAL_MESH_LOADER_HPP
#define ASHKAL_MESH_LOADER_HPP
#include <exception>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Ashkal/Ashkal.hpp"
#include "Ashkal/Material.hpp"
#include "Ashkal/Mesh.hpp"
#include "Ashkal/SolidColorSampler.hpp"
#include "Ashkal/VertexTriangle.hpp"

namespace Ashkal {
  inline Mesh load_mesh(const std::filesystem::path& path) {
    auto importer = Assimp::Importer();
    auto scene = importer.ReadFile(path.string().c_str(),
      aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
      aiProcess_GenNormals | aiProcess_ImproveCacheLocality);
    if(!scene || !scene->HasMeshes()) {
      throw std::runtime_error(
        "Failed to load OBJ: " + std::string(importer.GetErrorString()));
    }
    auto vertices = std::vector<Vertex>();
    auto triangles = std::vector<VertexTriangle>();
    for(auto i = std::size_t(0); i < scene->mNumMeshes; ++i) {
      auto mesh = scene->mMeshes[i];
      auto mesh_vertex_indices = std::vector<std::uint16_t>();
      for(auto j = std::size_t(0); j < mesh->mNumVertices; ++j) {
        auto& pos = mesh->mVertices[j];
        auto position = Point(pos.x, pos.y, pos.z);
        auto normal = Vector(0, 0, 1);
        if(mesh->HasNormals()) {
          auto& n = mesh->mNormals[j];
          normal = Vector(n.x, n.y, n.z);
        }
        auto uv = TextureCoordinate(0, 0);
        if(mesh->HasTextureCoords(0)) {
          uv.m_u = mesh->mTextureCoords[0][j].x;
          uv.m_v = mesh->mTextureCoords[0][j].y;
        }
        vertices.push_back({position, uv, normal});
        mesh_vertex_indices.push_back(
          static_cast<std::uint16_t>(vertices.size() - 1));
      }
      for(auto j = std::size_t(0); j < mesh->mNumFaces; ++j) {
        auto& face = mesh->mFaces[j];
        if(face.mNumIndices != 3) {
          throw std::runtime_error("Non-triangular face found.");
        }
        triangles.push_back({
          mesh_vertex_indices[face.mIndices[0]],
          mesh_vertex_indices[face.mIndices[1]],
          mesh_vertex_indices[face.mIndices[2]]
        });
      }
    }
    auto sampler = std::make_shared<SolidColorSampler>(Color(255, 0, 0));
    auto material = std::make_shared<Material>(sampler);
    auto fragment = Fragment(std::move(triangles), material);
    auto root = MeshNode(std::move(fragment));
    return Mesh(std::move(vertices), std::move(root));
  }
}

#endif
