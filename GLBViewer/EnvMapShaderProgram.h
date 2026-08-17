#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class EnvMapShaderProgram : public ShaderProgram
  {
   public:
    EnvMapShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;
    void setEquirectangularMap(const Texture2D& texture) const;

   private:
    int mView;
    int mProjection;
    int mEquirectangularMap;
  };
}
