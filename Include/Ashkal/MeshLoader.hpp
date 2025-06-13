#ifndef ASHKAL_MESH_LOADER_HPP
#define ASHKAL_MESH_LOADER_HPP
#include <filesystem>
#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <SDL_image.h>
#include "Ashkal/Material.hpp"
#include "Ashkal/Mesh.hpp"
#include "Ashkal/SdlSurfaceColorSampler.hpp"
#include "Ashkal/SolidColorSampler.hpp"
#include "Ashkal/VertexTriangle.hpp"

namespace Ashkal {

  /**
   * Loads a Mesh from a file on disk.
   * @param path The path to the mesh file to load (e.g., .obj, .ply).
   * @return A Mesh populated with vertices and a root MeshNode.
   * @throws std::runtime_error if the file cannot be read or parsing fails.
   */
  inline Mesh load_mesh(const std::filesystem::path& path) {
    auto importer = Assimp::Importer();
    auto scene = importer.ReadFile(path.string(), aiProcess_Triangulate |
      aiProcess_JoinIdenticalVertices | aiProcess_GenNormals |
      aiProcess_ImproveCacheLocality | aiProcess_PreTransformVertices);
    if(!scene || !scene->HasMeshes()) {
      throw std::runtime_error(
        "Failed to load OBJ: " + std::string(importer.GetErrorString()));
    }
    auto vertices = std::vector<Vertex>();
    auto children = std::vector<MeshNode>();
    auto base_directory = path.parent_path();
    for(auto i = std::size_t(0); i < scene->mNumMeshes; ++i) {
      auto mesh = scene->mMeshes[i];
      auto vertex_indices = std::vector<std::uint16_t>();
      for(auto j = std::size_t(0); j < mesh->mNumVertices; ++j) {
        auto& p = mesh->mVertices[j];
        auto position = Point(p.x, p.y, p.z);
        auto normal = [&] {
          if(mesh->HasNormals()) {
            auto& normal = mesh->mNormals[j];
            return Vector(normal.x, normal.y, normal.z);
          }
          return Vector(0, 0, 1);
        }();
        auto uv = [&] {
          if(mesh->HasTextureCoords(0)) {
            return TextureCoordinate(mesh->mTextureCoords[0][j].x,
              mesh->mTextureCoords[0][j].y);
          }
          return TextureCoordinate(0, 0);
        }();
        vertices.push_back({position, uv, normal});
        vertex_indices.push_back(
          static_cast<std::uint16_t>(vertices.size() - 1));
      }
      auto triangles = std::vector<VertexTriangle>();
      for(auto j = std::size_t(0); j < mesh->mNumFaces; ++j) {
        auto& face = mesh->mFaces[j];
        if(face.mNumIndices != 3) {
          throw std::runtime_error("Non-triangular face found.");
        }
        triangles.push_back({vertex_indices[face.mIndices[0]],
          vertex_indices[face.mIndices[1]], vertex_indices[face.mIndices[2]]});
      }
      auto sampler = [&] () -> std::shared_ptr<ColorSampler> {
        auto material = [&] () -> aiMaterial* {
          if(mesh->mMaterialIndex < scene->mNumMaterials) {
            return scene->mMaterials[mesh->mMaterialIndex];
          }
          return nullptr;
        }();
        if(material) {
          auto texture_path = aiString();
          if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) ==
              AI_SUCCESS) {
            try {
              return load_sampler(base_directory / texture_path.C_Str());
            } catch(const std::exception&) {
              auto diffuse_color = aiColor3D(1, 1, 1);
              material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
              auto color = Color(
                static_cast<std::uint8_t>(diffuse_color.r * 255),
                static_cast<std::uint8_t>(diffuse_color.g * 255),
                static_cast<std::uint8_t>(diffuse_color.b * 255), 255);
              return std::make_shared<SolidColorSampler>(color);
            }
          }
          auto diffuse_color = aiColor3D(1, 1, 1);
          material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
          auto color = Color(static_cast<std::uint8_t>(diffuse_color.r * 255),
            static_cast<std::uint8_t>(diffuse_color.g * 255),
            static_cast<std::uint8_t>(diffuse_color.b * 255), 255);
          return std::make_shared<SolidColorSampler>(color);
        }
        return std::make_shared<SolidColorSampler>(Color(255, 255, 255, 255));
      }();
      auto material = std::make_shared<Material>(sampler);
      auto fragment = Fragment(std::move(triangles), material);
      children.emplace_back(std::move(fragment));
    }
    auto root = MeshNode(std::move(children));
    return Mesh(std::move(vertices), std::move(root));
  }
}

#endif
