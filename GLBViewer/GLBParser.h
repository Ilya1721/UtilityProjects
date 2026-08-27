#pragma once

#include <fastgltf/core.hpp>
#include <filesystem>

#include "Image.h"
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
    std::shared_ptr<Texture2D> parseTexture(
      const fastgltf::TextureInfo& textureInfo, bool useSRGB, int colorChannels = 0
    ) const;
    std::unique_ptr<RegularImage> loadImageFromMemory(
      const fastgltf::sources::BufferView& bufferViewSource, int colorChannels
    ) const;
    std::vector<Vertex> parseVertices(const fastgltf::Node& node) const;

   private:
    fastgltf::Asset* mAsset = nullptr;
    mutable std::unordered_map<size_t, std::shared_ptr<Texture2D>> mTextureMap;
  };
}
