#include "EnvMapShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int EQUIRECTANGULAR_MAP_UNIT = 0;
}

namespace GLBViewer
{
  EnvMapShaderProgram::EnvMapShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mView = getUniformLocation("view");
    mProjection = getUniformLocation("projection");
    mEquirectangularMap = getUniformLocation("equirectangularMap");
  }

  void EnvMapShaderProgram::setView(const glm::mat4& view) const
  {
    bind();
    glUniformMatrix4fv(mView, 1, false, glm::value_ptr(view));
  }

  void EnvMapShaderProgram::setProjection(const glm::mat4& projection) const
  {
    bind();
    glUniformMatrix4fv(mProjection, 1, false, glm::value_ptr(projection));
  }

  void EnvMapShaderProgram::setEquirectangularMap(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mEquirectangularMap, EQUIRECTANGULAR_MAP_UNIT);
    glBindTextureUnit(EQUIRECTANGULAR_MAP_UNIT, texture.getId());
  }
}
