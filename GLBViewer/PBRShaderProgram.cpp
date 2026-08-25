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
  constexpr int IRRADIANCE_MAP_UNIT = 3;
  constexpr int PREFILTERED_ENV_MAP_UNIT = 4;
  constexpr int BRDF_LUT_UNIT = 5;
  constexpr int TRANSMISSION_TEXTURE_UNIT = 6;
  constexpr int OPAQUE_OFFSCREEN_TEXTURE_UNIT = 7;
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
    mCameraPosition = getUniformLocation("cameraPosition");
    mBaseColor.factor = getUniformLocation("baseColorFactor");
    mBaseColor.unit = getUniformLocation("baseColorTexture");
    mBaseColor.isAvailable = getUniformLocation("hasBaseColorTexture");
    mNormalMap.unit = getUniformLocation("normalMap");
    mNormalMap.isAvailable = getUniformLocation("hasNormalMap");
    mMetallicRoughness.metallicFactor = getUniformLocation("metallicFactor");
    mMetallicRoughness.roughnessFactor = getUniformLocation("roughnessFactor");
    mMetallicRoughness.isAvailable = getUniformLocation("hasMetallicRoughness");
    mMetallicRoughness.unit = getUniformLocation("metallicRoughness");
    mTransmission.factor = getUniformLocation("transmissionFactor");
    mTransmission.unit = getUniformLocation("transmissionTexture");
    mTransmission.isAvailable = getUniformLocation("hasTransmissionTexture");
    mPrefilteredEnvMap = getUniformLocation("prefilteredEnvMap");
    mIrradianceMap = getUniformLocation("irradianceMap");
    mBRDFLUT = getUniformLocation("brdfLUT");
    mOpaqueOffscreen = getUniformLocation("opaqueOffscreen");
    mIOR = getUniformLocation("ior");
    mAlphaCutoff = getUniformLocation("alphaCutoff");
    mAlphaMode = getUniformLocation("alphaMode");
    mViewportSize = getUniformLocation("viewportSize");
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

  void PBRShaderProgram::setMaterial(const PBRMaterial& material) const
  {
    bind();
    setTexture(mBaseColor, material.baseColorTexture.get(), BASE_COLOR_TEXTURE_UNIT);
    glUniform4fv(mBaseColor.factor, 1, glm::value_ptr(material.baseColorFactor));
    setTexture(mNormalMap, material.normalMap.get(), NORMAL_MAP_TEXTURE_UNIT);
    setTexture(
      mMetallicRoughness, material.metallicRougnessTexture.get(),
      METALLIC_ROUGHNESS_TEXTURE_UNIT
    );
    glUniform1f(mMetallicRoughness.metallicFactor, material.metallicFactor);
    glUniform1f(mMetallicRoughness.roughnessFactor, material.rougnessFactor);
    setTexture(
      mTransmission, material.transmissiveTexture.get(), TRANSMISSION_TEXTURE_UNIT
    );
    glUniform1f(mTransmission.factor, material.transmissionFactor);
    glUniform1f(mIOR, material.ior);
    glUniform1f(mAlphaCutoff, material.alphaCutoff);
    glUniform1i(mAlphaMode, static_cast<int>(material.alphaMode));
  }

  void PBRShaderProgram::setLightDir(const glm::vec3& lightDir) const
  {
    glUniform3fv(mLightDir, 1, glm::value_ptr(lightDir));
  }

  void PBRShaderProgram::setCameraPosition(const glm::vec3& cameraPosition) const
  {
    glUniform3fv(mCameraPosition, 1, glm::value_ptr(cameraPosition));
  }

  void PBRShaderProgram::setIrradianceMap(const CubemapTexture& texture) const
  {
    bind();
    glUniform1i(mIrradianceMap, IRRADIANCE_MAP_UNIT);
    glBindTextureUnit(IRRADIANCE_MAP_UNIT, texture.getId());
  }

  void PBRShaderProgram::setPrefilteredEnvMap(const CubemapTexture& texture) const
  {
    bind();
    glUniform1i(mPrefilteredEnvMap, PREFILTERED_ENV_MAP_UNIT);
    glBindTextureUnit(PREFILTERED_ENV_MAP_UNIT, texture.getId());
  }

  void PBRShaderProgram::setBRDFLUT(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mBRDFLUT, BRDF_LUT_UNIT);
    glBindTextureUnit(BRDF_LUT_UNIT, texture.getId());
  }

  void PBRShaderProgram::setOpaqueOffscreen(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mOpaqueOffscreen, OPAQUE_OFFSCREEN_TEXTURE_UNIT);
    glBindTextureUnit(OPAQUE_OFFSCREEN_TEXTURE_UNIT, texture.getId());
  }

  void PBRShaderProgram::setViewportSize(const glm::vec2& viewportSize) const
  {
    bind();
    glUniform2fv(mViewportSize, 1, glm::value_ptr(viewportSize));
  }
}
