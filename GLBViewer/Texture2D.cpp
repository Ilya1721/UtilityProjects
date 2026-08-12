#include "Texture2D.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  Texture2D::Texture2D(int width, int height) noexcept : mWidth(width), mHeight(height)
  {
    glGenTextures(1, &mTexture);
  }

  Texture2D::~Texture2D()
  {
    glDeleteTextures(1, &mTexture);
  }

  void Texture2D::bind() const
  {
    glActiveTexture(mTexture);
  }

  int Texture2D::getId() const
  {
    return mTexture;
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
