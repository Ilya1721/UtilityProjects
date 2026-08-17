#pragma once

namespace GLBViewer
{
  class FrameBuffer
  {
   public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void bind() const;

   private:
    unsigned int mFBO {};
    unsigned int mRBO {};
  };
}
