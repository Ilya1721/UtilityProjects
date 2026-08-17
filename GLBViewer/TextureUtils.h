#pragma once

#include <stb_image/stb_image.h>

#include <memory>
#include <string>

#include "CubemapTexture.h"
#include "Texture2D.h"

namespace GLBViewer
{
  template <typename T> struct Image
  {
    ~Image()
    {
      stbi_image_free(data);
    }
    int width {};
    int height {};
    int colorChannels {};
    T* data = nullptr;
  };

  using RegularImage = Image<unsigned char>;
  using HDRI = Image<float>;

  std::shared_ptr<Texture2D> createImageTexture(
    const RegularImage& image, bool useGammaCorrection
  );
  std::unique_ptr<CubemapTexture> loadEnvCubemap(const std::string& filePath);
  std::unique_ptr<RegularImage> loadImage(const std::string& filePath);
  std::unique_ptr<RegularImage> loadImage(const unsigned char* bytes, int bytesLength);
}
