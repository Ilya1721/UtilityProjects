#include "Texture2D.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  Texture2D::Texture2D(int width, int height) : Texture(), mWidth(width), mHeight(height)
  {
  }

  void Texture2D::bind() const
  {
    glBindTexture(GL_TEXTURE_2D, mTexture);
  }

  int Texture2D::getWidth() const
  {
    return mWidth;
  }

  int Texture2D::getHeight() const
  {
    return mHeight;
  }

  ResizableTexture2D::ResizableTexture2D(
    int width, int height, int internalFormat, int format, int type, const void* data
  )
    : Texture2D(width, height),
      mInternalFormat(internalFormat),
      mFormat(format),
      mType(type),
      mData(data)
  {
    create();
  }

  void ResizableTexture2D::resize(int width, int height)
  {
    mWidth = width;
    mHeight = height;
    create();
  }

  void ResizableTexture2D::create()
  {
    bind();
    glTexImage2D(
      GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, mData
    );
  }
}
