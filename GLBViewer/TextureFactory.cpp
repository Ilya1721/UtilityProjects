#include "TextureFactory.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "Constants.h"
#include "TextureUtils.h"

namespace
{
  using namespace GLBViewer;

  int getColorFormat(int colorChannels)
  {
    switch (colorChannels)
    {
      case 1:
        return GL_RED;
      case 2:
        return GL_RG;
      case 3:
        return GL_RGB;
      case 4:
        return GL_RGBA;
      default:
        return GL_RGB;
    }
  }

  std::shared_ptr<Texture2D> createImageTexture(const Image& image)
  {
    auto texture = std::make_shared<Texture2D>(image.width, image.height);
    texture->bind();
    const auto colorFormat = getColorFormat(image.colorChannels);
    glTexImage2D(
      GL_TEXTURE_2D, 0, colorFormat, image.width, image.height, 0, colorFormat,
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
}

namespace GLBViewer
{
  std::shared_ptr<Texture2D> createImageTexture(const std::string& filePath)
  {
    auto image = loadImage(filePath);
    return ::createImageTexture(*image);
  }

  std::shared_ptr<Texture2D> createImageTexture(
    const unsigned char* bytes, int bytesLength
  )
  {
    auto image = loadImage(bytes, bytesLength);
    return ::createImageTexture(*image);
  }
}
