#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class SMAABlendShaderProgram : public ShaderProgram
  {
   public:
    SMAABlendShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );
    void setEdgesTexture(const Texture2D& texture) const;
    void setAreaTexture(const Texture2D& texture) const;
    void setSearchTexture(const Texture2D& texture) const;
    void setTexelSize(const glm::vec2& size) const;
    void setViewportSize(const glm::vec2& size) const;

   private:
    int mEdgesTexture;
    int mTexelSize;
    int mViewportSize;
    int mAreaTexture;
    int mSearchTexture;
  };
}
