#include "TextureUtils.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Constants.h"

namespace
{
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
        throw std::exception("Unsupported color format");
    }
  }
}

namespace GLBViewer
{
  Image::~Image()
  {
    stbi_image_free(data);
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
