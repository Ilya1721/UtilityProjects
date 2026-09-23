#pragma once

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class ACESToneMappingShaderProgram : public ShaderProgram
  {
   public:
    ACESToneMappingShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );
    void setHDRTexture(const Texture2D& texture) const;

   private:
    int mHDRTexture;
  };
}
