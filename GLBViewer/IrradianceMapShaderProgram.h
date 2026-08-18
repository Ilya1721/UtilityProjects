#pragma once

#include <glm/glm.hpp>

#include "CubemapTexture.h"
#include "ShaderProgram.h"

namespace GLBViewer
{
  class IrradianceMapShaderProgram : public ShaderProgram
  {
   public:
    IrradianceMapShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;
    void setEnvMap(const CubemapTexture& texture) const;

   private:
    int mView;
    int mProjection;
    int mEnvMap;
  };
}
