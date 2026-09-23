#include "ACESToneMappingShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace
{
  constexpr int HDR_TEXTURE_UNIT = 0;
}

namespace GLBViewer
{
  ACESToneMappingShaderProgram::ACESToneMappingShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mHDRTexture = getUniformLocation("hdrTexture");
  }

  void ACESToneMappingShaderProgram::setHDRTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mHDRTexture, HDR_TEXTURE_UNIT);
    glBindTextureUnit(HDR_TEXTURE_UNIT, texture.getId());
  }
}
