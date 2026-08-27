#pragma once

#include <memory>
#include <string>

#include "CubemapTexture.h"
#include "Image.h"
#include "Texture2D.h"

namespace GLBViewer
{
  int getFormat(int colorChannels);
  int getInternalFormat(int colorChannels, bool useSRGB);
  void onScreenTextureResized(const Texture2D& texture, int width, int height);
  std::unique_ptr<Texture2D> createScreenTexture(int width, int height);
  std::unique_ptr<RegularImage> loadImage(
    const std::string& filePath, int colorChannels = 0
  );
  std::unique_ptr<RegularImage> loadImage(
    const unsigned char* bytes, int bytesLength, int colorChannels = 0
  );
  std::unique_ptr<HDRI> loadHDRI(const std::string& filePath);
}
