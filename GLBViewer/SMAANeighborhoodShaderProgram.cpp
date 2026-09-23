#include "SMAANeighborhoodShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int FRAME_UNIT = 0;
  constexpr int BLEND_TEXTURE_UNIT = 1;
}

namespace GLBViewer
{
  SMAANeighborhoodShaderProgram::SMAANeighborhoodShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mFrame = getUniformLocation("frame");
    mBlendTexture = getUniformLocation("blendTexture");
    mTexelSize = getUniformLocation("texelSize");
  }

  void SMAANeighborhoodShaderProgram::setFrame(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mFrame, FRAME_UNIT);
    glBindTextureUnit(FRAME_UNIT, texture.getId());
  }

  void SMAANeighborhoodShaderProgram::setBlendTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mBlendTexture, BLEND_TEXTURE_UNIT);
    glBindTextureUnit(BLEND_TEXTURE_UNIT, texture.getId());
  }

  void SMAANeighborhoodShaderProgram::setTexelSize(const glm::vec2& texelSize) const
  {
    bind();
    glUniform2fv(mTexelSize, 1, glm::value_ptr(texelSize));
  }
}