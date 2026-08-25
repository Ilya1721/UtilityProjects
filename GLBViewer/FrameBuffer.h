#pragma once

#include "CubemapTexture.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class FrameBuffer
  {
   public:
    FrameBuffer();
    ~FrameBuffer();

    void bind() const;
    void addColorAttachment(const Texture2D& texture, int mipLevel) const;
    void addColorAttachment(const CubemapTexture& texture, int faceIdx, int mipLevel) const;
    void addDepthAttachment(int width, int height) const;
    void copyPixels(int width, int height, int targetFBO, int mask) const;

   private:
    unsigned int mFBO {};
    unsigned int mRBO {};
  };
}
