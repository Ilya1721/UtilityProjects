#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class SMAAEdgeShaderProgram : public ShaderProgram
  {
   public:
    SMAAEdgeShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setFrame(const Texture2D& texture) const;
    void setTexelSize(const glm::vec2& texelSize) const;
    void setThreshold(float threshold) const;
    void setLocalContrastAdaptationFactor(float factor) const;

   private:
    int mFrame;
    int mTexelSize;
    int mThreshold;
    int mLocalContrastAdaptationFactor;
  };
}
