#include "IBL.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"
#include "EnvMapShaderProgram.h"
#include "FrameBuffer.h"
#include "Image.h"
#include "IrradianceMapShaderProgram.h"
#include "PrefilteredEnvMapShaderProgram.h"

namespace
{
  using namespace GLBViewer;

  std::unique_ptr<HDRI> loadHDRI(const std::string& filePath)
  {
    auto image = std::make_unique<HDRI>();
    stbi_set_flip_vertically_on_load(true);
    image->data = stbi_loadf(
      filePath.c_str(), &image->width, &image->height, &image->colorChannels, 0
    );
    stbi_set_flip_vertically_on_load(false);
    return image;
  }

  void initCubemapTexture()
  {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  void initTexture2D()
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  std::unique_ptr<Texture2D> createHDRTexture(const HDRI& image)
  {
    auto texture = std::make_unique<Texture2D>(image.width, image.height);
    texture->bind();
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB16F, image.width, image.height, 0, GL_RGB, GL_FLOAT,
      image.data
    );
    initTexture2D();
    return texture;
  }

  std::unique_ptr<CubemapTexture> createCubemap(int size)
  {
    auto texture = std::make_unique<CubemapTexture>();
    texture->bind();
    for (size_t faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
      glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx, 0, GL_RGB16F, size, size, 0, GL_RGB,
        GL_FLOAT, nullptr
      );
    }
    initCubemapTexture();
    return texture;
  }

  std::unique_ptr<CubemapTexture> createPrefilteredCubemap()
  {
    auto texture = std::make_unique<CubemapTexture>();
    texture->bind();
    for (size_t level = 0; level < PREFILTERED_MAP_MAX_MIP_LEVELS; ++level)
    {
      size_t levelSize = PREFILTERED_MAP_SIZE * std::pow(0.5, level);
      for (size_t face = 0; face < 6; ++face)
      {
        glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, GL_RGB16F, levelSize, levelSize,
          0, GL_RGB, GL_FLOAT, nullptr
        );
      }
    }
    initCubemapTexture();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(
      GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, PREFILTERED_MAP_MAX_MIP_LEVELS - 1
    );
    return texture;
  }

  std::unique_ptr<Texture2D> createBRDFLUTTexture()
  {
    auto texture = std::make_unique<Texture2D>(BRDF_LUT_SIZE, BRDF_LUT_SIZE);
    texture->bind();
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RG16F, BRDF_LUT_SIZE, BRDF_LUT_SIZE, 0, GL_RG, GL_FLOAT,
      nullptr
    );
    initTexture2D();
    return texture;
  }

  const glm::mat4 CAPTURE_VIEWS[] = {
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)
    ),
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)
    ),
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    ),
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)
    ),
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)
    ),
    glm::lookAt(
      glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)
    )
  };

  const glm::mat4 CAPTURE_PROJECTION =
    glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
}

namespace GLBViewer
{
  IBL::IBL(const std::string& hdriFilePath)
  {
    auto hdri = loadHDRI(hdriFilePath);
    auto hdrTexture = createHDRTexture(*hdri);
    mCubeRenderBuffer = std::make_unique<CubeRenderBuffer>();
    mCubeRenderBuffer->sendDataToGPU();
    mEnvMap = loadEnvMap(*hdrTexture);
  }

  std::unique_ptr<CubemapTexture> IBL::loadCubemap(
    int mapSize, const std::function<void(const glm::mat4& view)>& viewSetter
  ) const
  {
    auto cubemap = createCubemap(mapSize);
    auto frameBuffer = std::make_unique<FrameBuffer>(mapSize, mapSize);
    frameBuffer->bind();
    mCubeRenderBuffer->bind();
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, mapSize, mapSize);
    for (size_t faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
      viewSetter(CAPTURE_VIEWS[faceIdx]);
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx,
        cubemap->getId(), 0
      );
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glViewport(0, 0, viewport[2], viewport[3]);
    return cubemap;
  }

  std::unique_ptr<CubemapTexture> IBL::loadEnvMap(const Texture2D& hdrTexture) const
  {
    EnvMapShaderProgram shader(ENV_MAP_VERTEX_SHADER_PATH, ENV_MAP_FRAGMENT_SHADER_PATH);
    shader.setProjection(CAPTURE_PROJECTION);
    shader.setEquirectangularMap(hdrTexture);
    shader.bind();
    auto viewSetter = [&shader](const glm::mat4& view) { shader.setView(view); };
    auto envMap = loadCubemap(ENV_MAP_SIZE, viewSetter);
    envMap->bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    return envMap;
  }

  std::unique_ptr<CubemapTexture> IBL::loadIrradianceMap() const
  {
    IrradianceMapShaderProgram shader(
      IRRADIANCE_MAP_VERTEX_SHADER_PATH, IRRADIANCE_MAP_FRAGMENT_SHADER_PATH
    );
    shader.setProjection(CAPTURE_PROJECTION);
    shader.setEnvMap(*mEnvMap);
    shader.bind();
    auto viewSetter = [&shader](const glm::mat4& view) { shader.setView(view); };
    return loadCubemap(IRRADIANCE_MAP_SIZE, viewSetter);
  }

  std::unique_ptr<CubemapTexture> IBL::loadPrefilteredEnvMap() const
  {
    auto cubemap = createPrefilteredCubemap();
    PrefilteredEnvMapShaderProgram shader(
      PREFILTERED_ENV_MAP_VERTEX_SHADER_PATH, PREFILTERED_ENV_MAP_FRAGMENT_SHADER_PATH
    );
    shader.setProjection(CAPTURE_PROJECTION);
    shader.setEnvMap(*mEnvMap);
    shader.bind();
    auto frameBuffer =
      std::make_unique<FrameBuffer>(PREFILTERED_MAP_SIZE, PREFILTERED_MAP_SIZE);
    frameBuffer->bind();
    mCubeRenderBuffer->bind();
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    for (size_t level = 0; level < PREFILTERED_MAP_MAX_MIP_LEVELS; ++level)
    {
      size_t levelSize = PREFILTERED_MAP_SIZE * std::pow(0.5, level);
      glViewport(0, 0, levelSize, levelSize);
      float roughness = level / static_cast<float>(PREFILTERED_MAP_MAX_MIP_LEVELS - 1);
      shader.setRougness(roughness);
      for (size_t faceIdx = 0; faceIdx < 6; ++faceIdx)
      {
        shader.setView(CAPTURE_VIEWS[faceIdx]);
        glFramebufferTexture2D(
          GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx,
          cubemap->getId(), level
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
    glViewport(0, 0, viewport[2], viewport[3]);
    return cubemap;
  }

  std::unique_ptr<Texture2D> IBL::loadBRDFLUT() const
  {
    auto texture = createBRDFLUTTexture();
    auto viewportWidth = texture->getWidth();
    auto viewportHeight = texture->getHeight();
    ShaderProgram shader(BRDF_LUT_VERTEX_SHADER_PATH, BRDF_LUT_FRAGMENT_SHADER_PATH);
    shader.bind();
    auto squareRenderBuffer = std::make_unique<SquareRenderBuffer>();
    squareRenderBuffer->sendDataToGPU();
    squareRenderBuffer->bind();
    auto frameBuffer = std::make_unique<FrameBuffer>(viewportWidth, viewportHeight);
    frameBuffer->bind();
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glDisable(GL_DEPTH_TEST);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getId(), 0
    );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, viewport[2], viewport[3]);
    return texture;
  }
}
