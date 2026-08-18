#pragma once

#include <stb_image/stb_image.h>

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
}
