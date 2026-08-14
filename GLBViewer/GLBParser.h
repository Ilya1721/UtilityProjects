#pragma once

#include <fastgltf/core.hpp>
#include <filesystem>

#include "Mesh.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class GLBParser
  {
   public:
    std::vector<std::unique_ptr<Mesh>> parseScene(const std::filesystem::path& scenePath);

   private:
    std::unique_ptr<Mesh> parseNode(const fastgltf::Node& node) const;
    std::unique_ptr<Mesh> parseMesh(const fastgltf::Node& node) const;
    PBRMaterial parseMaterial(const fastgltf::Node& node) const;
    std::shared_ptr<Texture2D> getTexture(
      const fastgltf::TextureInfo& textureInfo, bool useGammaCorrection
    ) const;
    std::shared_ptr<Texture2D> loadTextureFromMemory(
      const fastgltf::sources::BufferView& bufferViewSource, bool useGammaCorrection
    ) const;
    std::shared_ptr<Texture2D> loadTextureFromFile(
      const char* filePath, bool useGammaCorrection
    ) const;
    std::vector<Vertex> parseVertices(const fastgltf::Node& node) const;

   private:
    fastgltf::Asset* mAsset = nullptr;
    mutable std::unordered_map<size_t, std::shared_ptr<Texture2D>> mTextureMap;
  };
}
