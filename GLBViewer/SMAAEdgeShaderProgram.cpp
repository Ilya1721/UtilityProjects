#include "SMAAEdgeShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int FRAME_UNIT = 0;
}

namespace GLBViewer
{
  SMAAEdgeShaderProgram::SMAAEdgeShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mFrame = getUniformLocation("frame");
    mTexelSize = getUniformLocation("texelSize");
    mThreshold = getUniformLocation("threshold");
    mLocalContrastAdaptationFactor = getUniformLocation("localContrastAdaptationFactor");
  }

  void SMAAEdgeShaderProgram::setFrame(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mFrame, FRAME_UNIT);
    glBindTextureUnit(FRAME_UNIT, texture.getId());
  }

  void SMAAEdgeShaderProgram::setTexelSize(const glm::vec2& texelSize) const
  {
    bind();
    glUniform2fv(mTexelSize, 1, glm::value_ptr(texelSize));
  }

  void SMAAEdgeShaderProgram::setThreshold(float threshold) const
  {
    bind();
    glUniform1f(mThreshold, threshold);
  }

  void SMAAEdgeShaderProgram::setLocalContrastAdaptationFactor(float factor) const
  {
    bind();
    glUniform1f(mLocalContrastAdaptationFactor, factor);
  }
}
