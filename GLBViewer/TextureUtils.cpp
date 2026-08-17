#include "TextureUtils.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"
#include "EnvMapShaderProgram.h"
#include "FrameBuffer.h"
#include "RenderBuffer.h"

namespace
{
  using namespace GLBViewer;

  int getInternalFormat(int colorChannels, bool useGammaCorrection)
  {
    if (useGammaCorrection)
    {
      return colorChannels == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8;
    }
    return colorChannels == 3 ? GL_RGB8 : GL_RGBA8;
  }

  std::shared_ptr<Texture2D> createHDRTexture(const HDRI& image)
  {
    auto texture = std::make_shared<Texture2D>(image.width, image.height);
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

  std::unique_ptr<CubemapTexture> createEnvCubemap()
  {
    auto texture = std::make_unique<CubemapTexture>();
    texture->bind();
    for (size_t faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
      glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx, 0, GL_RGB16F, ENV_MAP_SIZE,
        ENV_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr
      );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
  }

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
  std::shared_ptr<Texture2D> createImageTexture(
    const RegularImage& image, bool useGammaCorrection
  )
  {
    auto texture = std::make_shared<Texture2D>(image.width, image.height);
    texture->bind();
    auto format = image.colorChannels == 3 ? GL_RGB : GL_RGBA;
    auto internalFormat = getInternalFormat(image.colorChannels, useGammaCorrection);
    glTexImage2D(
      GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format,
      GL_UNSIGNED_BYTE, image.data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    float maxAnisotropyLevel = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropyLevel);
    float anisotropyLevel = std::min(ANISOTROPIC_FILTERING_LEVEL, maxAnisotropyLevel);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel);
    return texture;
  }

  std::unique_ptr<CubemapTexture> loadEnvCubemap(const std::string& filePath)
  {
    auto hdri = loadHDRI(filePath);
    auto hdrTexture = createHDRTexture(*hdri);
    auto envCubemap = createEnvCubemap();
    EnvMapShaderProgram shader(ENV_MAP_VERTEX_SHADER_PATH, ENV_MAP_FRAGMENT_SHADER_PATH);
    shader.setProjection(CAPTURE_PROJECTION);
    shader.setEquirectangularMap(*hdrTexture);
    CubeRenderBuffer renderBuffer;
    renderBuffer.sendDataToGPU();
    FrameBuffer frameBuffer(hdri->width, hdri->height);
    frameBuffer.bind();
    shader.bind();
    renderBuffer.bind();
    for (size_t faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
      shader.setView(CAPTURE_VIEWS[faceIdx]);
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx,
        envCubemap->getId(), 0
      );
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    return envCubemap;
  }

  std::unique_ptr<RegularImage> loadImage(const std::string& filePath)
  {
    auto image = std::make_unique<RegularImage>();
    image->data = stbi_load(
      filePath.c_str(), &image->width, &image->height, &image->colorChannels, 0
    );
    return image;
  }

  std::unique_ptr<RegularImage> loadImage(const unsigned char* bytes, int bytesLength)
  {
    auto image = std::make_unique<RegularImage>();
    image->data = stbi_load_from_memory(
      bytes, bytesLength, &image->width, &image->height, &image->colorChannels, 0
    );
    return image;
  }
}
