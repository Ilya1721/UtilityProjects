#pragma once

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class ScreenShaderProgram : public ShaderProgram
  {
   public:
    ScreenShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );
    void setScreenTexture(const Texture2D& texture) const;

   private:
    int mScreenTexture;
  };
}
