#pragma once

#include "Texture.h"

namespace GLBViewer
{
  class CubemapTexture : public Texture
  {
   public:
    void bind() const;
  };
}
