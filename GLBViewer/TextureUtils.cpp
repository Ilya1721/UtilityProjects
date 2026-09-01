#include "TextureUtils.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "Constants.h"

namespace GLBViewer
{
  int getFormat(int colorChannels)
  {
    switch (colorChannels)
    {
      case 1:
        return GL_R;
      case 2:
        return GL_RG;
      case 3:
        return GL_RGB;
      case 4:
        return GL_RGBA;
    }
    throw std::exception("Unsupported amount of color channels");
  }

  int getInternalFormat(int colorChannels, bool useSRGB)
  {
    if (useSRGB)
    {
      switch (colorChannels)
      {
        case 3:
          return GL_SRGB8;
        case 4:
          return GL_SRGB8_ALPHA8;
      }
      throw std::exception("Unsupported amount of color channels for SRGB");
    }
    switch (colorChannels)
    {
      case 1:
        return GL_R8;
      case 2:
        return GL_RG8;
      case 3:
        return GL_RGB8;
      case 4:
        return GL_RGBA8;
    }
    throw std::exception("Unsupported amount of color channels");
  }

  std::unique_ptr<ResizableTexture2D> createScreenTexture(int width, int height)
  {
    auto texture = std::make_unique<ResizableTexture2D>(
      width, height, GL_RGB16F, GL_RGB, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texture;
  }

  std::unique_ptr<ResizableTexture2D> createMotionVectorsTexture(int width, int height)
  {
    auto texture = std::make_unique<ResizableTexture2D>(
      width, height, GL_RG16F, GL_RG, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
  }

  std::unique_ptr<ResizableTexture2D> createDepthMap(int width, int height)
  {
    auto texture = std::make_unique<ResizableTexture2D>(
      width, height, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, DEPTH_TEXTURE_BORDER_COLOR);
    return texture;
  }

  std::unique_ptr<RegularImage> loadImage(const std::string& filePath, int colorChannels)
  {
    auto image = std::make_unique<RegularImage>();
    int fileColorChannels {};
    image->data = stbi_load(
      filePath.c_str(), &image->width, &image->height, &fileColorChannels, colorChannels
    );
    image->colorChannels = colorChannels != 0 ? colorChannels : fileColorChannels;
    return image;
  }

  std::unique_ptr<RegularImage> loadImage(
    const unsigned char* bytes, int bytesLength, int colorChannels
  )
  {
    auto image = std::make_unique<RegularImage>();
    int fileColorChannels {};
    image->data = stbi_load_from_memory(
      bytes, bytesLength, &image->width, &image->height, &fileColorChannels, colorChannels
    );
    image->colorChannels = colorChannels != 0 ? colorChannels : fileColorChannels;
    return image;
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
}
