#include "IBL.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"
#include "EnvMapShaderProgram.h"
#include "Image.h"
#include "IrradianceMapShaderProgram.h"

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

  std::unique_ptr<Texture2D> createHDRTexture(const HDRI& image)
  {
    auto texture = std::make_unique<Texture2D>(image.width, image.height);
    texture->bind();
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB16F, image.width, image.height, 0, GL_RGB, GL_FLOAT,
      image.data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    mFrameBuffer =
      std::make_unique<FrameBuffer>(hdrTexture->getWidth(), hdrTexture->getHeight());
    mRenderBuffer = std::make_unique<CubeRenderBuffer>();
    mRenderBuffer->sendDataToGPU();
    mEnvMap = loadEnvMap(*hdrTexture);
  }

  std::unique_ptr<CubemapTexture> IBL::loadCubemap(
    int mapSize, const std::function<void(const glm::mat4& view)>& viewSetter
  ) const
  {
    mFrameBuffer->bind();
    mRenderBuffer->bind();
    auto cubemap = createCubemap(mapSize);
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
    return cubemap;
  }

  std::unique_ptr<CubemapTexture> IBL::loadEnvMap(const Texture2D& hdrTexture) const
  {
    EnvMapShaderProgram shader(ENV_MAP_VERTEX_SHADER_PATH, ENV_MAP_FRAGMENT_SHADER_PATH);
    shader.setProjection(CAPTURE_PROJECTION);
    shader.setEquirectangularMap(hdrTexture);
    shader.bind();
    auto viewSetter = [&shader](const glm::mat4& view) { shader.setView(view); };
    return loadCubemap(ENV_MAP_SIZE, viewSetter);
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
}
