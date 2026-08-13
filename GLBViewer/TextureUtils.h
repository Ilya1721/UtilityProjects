#pragma once

#include <memory>
#include <string>

#include "Texture2D.h"

namespace GLBViewer
{
  struct Image
  {
    ~Image();
    int width {};
    int height {};
    int colorChannels {};
    unsigned char* data = nullptr;
  };

  std::shared_ptr<Texture2D> createImageTexture(const Image& image);
  std::unique_ptr<Image> loadImage(const std::string& filePath);
  std::unique_ptr<Image> loadImage(const unsigned char* bytes, int bytesLength);
}
