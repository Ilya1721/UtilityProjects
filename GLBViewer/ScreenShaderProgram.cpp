#include "ScreenShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace
{
  constexpr int SCREEN_TEXTURE_UNIT = 0;
}

namespace GLBViewer
{
  ScreenShaderProgram::ScreenShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mScreenTexture = getUniformLocation("screenTexture");
  }

  void ScreenShaderProgram::setScreenTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mScreenTexture, SCREEN_TEXTURE_UNIT);
    glBindTextureUnit(SCREEN_TEXTURE_UNIT, texture.getId());
  }
}