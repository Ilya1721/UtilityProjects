#include "SMAABlendShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int EDGES_TEXTURE_UNIT = 0;
  constexpr int AREA_TEXTURE_UNIT = 1;
  constexpr int SEARCH_TEXTURE_UNIT = 2;
}

namespace GLBViewer
{
  SMAABlendShaderProgram::SMAABlendShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mEdgesTexture = getUniformLocation("edgesTexture");
    mAreaTexture = getUniformLocation("areaTexture");
    mSearchTexture = getUniformLocation("searchTexture");
    mTexelSize = getUniformLocation("texelSize");
    mViewportSize = getUniformLocation("viewportSize");
  }

  void SMAABlendShaderProgram::setEdgesTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mEdgesTexture, EDGES_TEXTURE_UNIT);
    glBindTextureUnit(EDGES_TEXTURE_UNIT, texture.getId());
  }

  void SMAABlendShaderProgram::setAreaTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mAreaTexture, AREA_TEXTURE_UNIT);
    glBindTextureUnit(AREA_TEXTURE_UNIT, texture.getId());
  }

  void SMAABlendShaderProgram::setSearchTexture(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mSearchTexture, SEARCH_TEXTURE_UNIT);
    glBindTextureUnit(SEARCH_TEXTURE_UNIT, texture.getId());
  }

  void SMAABlendShaderProgram::setTexelSize(const glm::vec2& size) const
  {
    bind();
    glUniform2fv(mTexelSize, 1, glm::value_ptr(size));
  }

  void SMAABlendShaderProgram::setViewportSize(const glm::vec2& size) const
  {
    bind();
    glUniform2fv(mViewportSize, 1, glm::value_ptr(size));
  }
}
