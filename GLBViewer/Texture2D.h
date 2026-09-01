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

   protected:
    int mWidth {};
    int mHeight {};
  };

  class ResizableTexture2D : public Texture2D
  {
   public:
    ResizableTexture2D(
      int width, int height, int internalFormat, int format, int type, const void* data
    );
    void resize(int width, int height);

  private:
    void create();

   private:
    int mInternalFormat {};
    int mFormat {};
    int mType {};
    const void* mData {};
  };
}
