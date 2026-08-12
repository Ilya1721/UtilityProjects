#pragma once

#include <memory>
#include <string>

#include "Texture2D.h"

namespace GLBViewer
{
  std::shared_ptr<Texture2D> createImageTexture(const std::string& filePath);
  std::shared_ptr<Texture2D> createImageTexture(const unsigned char* bytes, int bytesLength);
}
