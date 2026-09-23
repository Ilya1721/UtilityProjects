#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class SMAANeighborhoodShaderProgram : public ShaderProgram
  {
  public:
    SMAANeighborhoodShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    void setFrame(const Texture2D& texture) const;
    void setBlendTexture(const Texture2D& texture) const;
    void setTexelSize(const glm::vec2& texelSize) const;

  private:
    int mFrame;
    int mBlendTexture;
    int mTexelSize;
  };
}
