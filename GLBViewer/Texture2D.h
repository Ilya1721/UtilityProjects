#pragma once

#include "Texture.h"

namespace GLBViewer
{
  class Texture2D : public Texture
  {
   public:
    Texture2D(int width, int height);
    int getWidth() const;
    int getHeight() const;
    void bind() const;

   private:
    int mWidth {};
    int mHeight {};
  };
}
