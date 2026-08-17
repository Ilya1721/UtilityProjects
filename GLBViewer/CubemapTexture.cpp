#include "CubemapTexture.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  void CubemapTexture::bind() const
  {
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
  }
}
