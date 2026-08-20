#include "PrefilteredEnvMapShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int ENV_MAP_UNIT = 0;
}

namespace GLBViewer
{
  PrefilteredEnvMapShaderProgram::PrefilteredEnvMapShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mView = getUniformLocation("view");
    mProjection = getUniformLocation("projection");
    mEnvMap = getUniformLocation("envMap");
    mRoughness = getUniformLocation("roughness");
  }

  void PrefilteredEnvMapShaderProgram::setView(const glm::mat4& view) const
  {
    bind();
    glUniformMatrix4fv(mView, 1, false, glm::value_ptr(view));
  }

  void PrefilteredEnvMapShaderProgram::setProjection(const glm::mat4& projection) const
  {
    bind();
    glUniformMatrix4fv(mProjection, 1, false, glm::value_ptr(projection));
  }

  void PrefilteredEnvMapShaderProgram::setEnvMap(const CubemapTexture& texture) const
  {
    bind();
    glUniform1i(mEnvMap, ENV_MAP_UNIT);
    glBindTextureUnit(ENV_MAP_UNIT, texture.getId());
  }

  void PrefilteredEnvMapShaderProgram::setRougness(float roughness) const
  {
    bind();
    glUniform1f(mRoughness, roughness);
  }
}
