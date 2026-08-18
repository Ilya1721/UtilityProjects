#pragma once

#include <memory>
#include <string>

#include "CubemapTexture.h"
#include "Image.h"
#include "Texture2D.h"

namespace GLBViewer
{
  std::shared_ptr<Texture2D> createImageTexture(
    const RegularImage& image, bool useGammaCorrection
  );
  std::unique_ptr<RegularImage> loadImage(const std::string& filePath);
  std::unique_ptr<RegularImage> loadImage(const unsigned char* bytes, int bytesLength);
}
