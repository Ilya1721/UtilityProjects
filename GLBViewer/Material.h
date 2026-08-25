#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Constants.h"
#include "Texture2D.h"

namespace GLBViewer
{
  enum class AlphaMode
  {
    OPAQUE,
    BLEND,
    MASK
  };

  struct PBRMaterial
  {
    std::shared_ptr<Texture2D> baseColorTexture;
    std::shared_ptr<Texture2D> normalMap;
    std::shared_ptr<Texture2D> metallicRougnessTexture;
    std::shared_ptr<Texture2D> transmissiveTexture;
    glm::vec4 baseColorFactor = DEFAULT_BASE_COLOR;
    float metallicFactor {};
    float rougnessFactor {};
    float transmissionFactor {};
    float ior {};
    float alphaCutoff {};
    AlphaMode alphaMode {};
  };
}
