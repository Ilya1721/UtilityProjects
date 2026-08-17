#include "Texture.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  Texture::Texture()
  {
    glGenTextures(1, &mTexture);
  }

  Texture::~Texture()
  {
    glDeleteTextures(1, &mTexture);
  }

  unsigned int Texture::getId() const
  {
    return mTexture;
  }
}
