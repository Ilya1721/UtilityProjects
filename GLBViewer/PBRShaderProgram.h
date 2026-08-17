#pragma once

#include <glm/glm.hpp>

#include "CubemapTexture.h"
#include "Material.h"
#include "ShaderProgram.h"

namespace GLBViewer
{
  struct TextureData
  {
    int unit {};
    int isAvailable {};
  };

  struct ColorTextureData : TextureData
  {
    int factor {};
  };

  struct MetallicRougnessTextureData : TextureData
  {
    int metallicFactor {};
    int roughnessFactor {};
  };

  class PBRShaderProgram : public ShaderProgram
  {
   public:
    PBRShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );
    void setModel(const glm::mat4& model) const;
    void setView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;
    void setMaterial(const PBRMaterial& material) const;
    void setLightDir(const glm::vec3& lightDir) const;
    void setCameraPosition(const glm::vec3& cameraPosition) const;
    void setEnvCubemap(const CubemapTexture& texture) const;

   private:
    void setTexture(TextureData& textureData, Texture2D* texture, int textureUnit) const;
    void setTexture(
      ColorTextureData& textureData,
      Texture2D* texture,
      const glm::vec4& factor,
      int textureUnit
    ) const;
    void setTexture(
      MetallicRougnessTextureData& textureData,
      Texture2D* texture,
      float metallicFactor,
      float roughnessFactor,
      int textureUnit
    ) const;

   private:
    int mModel;
    int mView;
    int mProjection;
    int mLightDir;
    int mCameraPosition;
    int mEnvMap;
    mutable TextureData mNormalMap;
    mutable ColorTextureData mBaseColor;
    mutable MetallicRougnessTextureData mMetallicRoughness;
  };
}
