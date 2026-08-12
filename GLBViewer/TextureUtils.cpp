#include "TextureUtils.h"

#include <stb_image/stb_image.h>

namespace GLBViewer
{
  Image::~Image()
  {
    stbi_image_free(data);
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
