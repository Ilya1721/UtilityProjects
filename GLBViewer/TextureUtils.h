#pragma once

#include <memory>
#include <string>

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

  std::unique_ptr<Image> loadImage(const std::string& filePath);
  std::unique_ptr<Image> loadImage(const unsigned char* bytes, int bytesLength);
}
