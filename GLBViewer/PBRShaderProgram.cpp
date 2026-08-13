#include "PBRShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int BASE_COLOR_TEXTURE_UNIT = 0;
  constexpr int NORMAL_MAP_TEXTURE_UNIT = 1;
  constexpr int METALLIC_ROUGHNESS_TEXTURE_UNIT = 2;
}

namespace GLBViewer
{
  PBRShaderProgram::PBRShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mModel = getUniformLocation("model");
    mView = getUniformLocation("view");
    mProjection = getUniformLocation("projection");
    mLightDir = getUniformLocation("lightDir");
    mCameraPos = getUniformLocation("cameraPos");
    mBaseColor.factor = getUniformLocation("baseColorFactor");
    mBaseColor.unit = getUniformLocation("baseColorTexture");
    mBaseColor.isAvailable = getUniformLocation("hasBaseColorTexture");
    mNormalMap.unit = getUniformLocation("normalMap");
    mNormalMap.isAvailable = getUniformLocation("hasNormalMap");
  }

  void PBRShaderProgram::setModel(const glm::mat4& model) const
  {
    bind();
    glUniformMatrix4fv(mModel, 1, false, glm::value_ptr(model));
  }

  void PBRShaderProgram::setView(const glm::mat4& view) const
  {
    bind();
    glUniformMatrix4fv(mView, 1, false, glm::value_ptr(view));
  }

  void PBRShaderProgram::setProjection(const glm::mat4& projection) const
  {
    bind();
    glUniformMatrix4fv(mProjection, 1, false, glm::value_ptr(projection));
  }

  void PBRShaderProgram::setTexture(
    TextureData& textureData, Texture2D* texture, int textureUnit
  ) const
  {
    glUniform1i(textureData.isAvailable, texture != nullptr);
    glUniform1i(textureData.unit, textureUnit);
    glBindTextureUnit(textureUnit, texture ? texture->getId() : 0);
  }

  void PBRShaderProgram::setTexture(
    ColorTextureData& textureData,
    Texture2D* texture,
    const glm::vec4& factor,
    int textureUnit
  ) const
  {
    setTexture(textureData, texture, textureUnit);
    glUniform4fv(textureData.factor, 1, glm::value_ptr(factor));
  }

  void PBRShaderProgram::setMaterial(const PBRMaterial& material) const
  {
    bind();
    setTexture(
      mBaseColor, material.baseColorTexture.get(), material.baseColorFactor,
      BASE_COLOR_TEXTURE_UNIT
    );
    setTexture(mNormalMap, material.normalMap.get(), NORMAL_MAP_TEXTURE_UNIT);
  }

  void PBRShaderProgram::setLightDir(const glm::vec3& lightDir) const
  {
    glUniform3fv(mLightDir, 1, glm::value_ptr(lightDir));
  }

  void PBRShaderProgram::setCameraPos(const glm::vec3& cameraPos) const
  {
    glUniform3fv(mCameraPos, 1, glm::value_ptr(cameraPos));
  }
}
