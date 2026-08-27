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

  void Texture2D::resize(int width, int height)
  {
    mWidth = width;
    mHeight = height;
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
}
