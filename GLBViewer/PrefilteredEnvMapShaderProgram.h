#pragma once

#include <glm/glm.hpp>

#include "CubemapTexture.h"
#include "ShaderProgram.h"

namespace GLBViewer
{
  class PrefilteredEnvMapShaderProgram : public ShaderProgram
  {
   public:
    PrefilteredEnvMapShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;
    void setEnvMap(const CubemapTexture& texture) const;
    void setRougness(float roughness) const;

   private:
     int mView;
     int mProjection;
     int mEnvMap;
     int mRoughness;
  };
}
