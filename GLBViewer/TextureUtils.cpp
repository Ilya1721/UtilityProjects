#include "TextureUtils.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Constants.h"

namespace
{
  int getInternalFormat(int colorChannels, bool useGammaCorrection)
  {
    if (useGammaCorrection)
    {
      return colorChannels == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8;
    }
    return colorChannels == 3 ? GL_RGB8 : GL_RGBA8;
  }
}

namespace GLBViewer
{
  Image::~Image()
  {
    stbi_image_free(data);
  }

  std::shared_ptr<Texture2D> createImageTexture(
    const Image& image, bool useGammaCorrection
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

  std::unique_ptr<Image> loadImage(const std::string& filePath)
  {
    auto image = std::make_unique<Image>();
    image->data = stbi_load(
      filePath.c_str(), &image->width, &image->height, &image->colorChannels, 0
    );
    return image;
  }

  std::unique_ptr<Image> loadImage(const unsigned char* bytes, int bytesLength)
  {
    auto image = std::make_unique<Image>();
    image->data = stbi_load_from_memory(
      bytes, bytesLength, &image->width, &image->height, &image->colorChannels, 0
    );
    return image;
  }
}
