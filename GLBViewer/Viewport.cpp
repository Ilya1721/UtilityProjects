#include "Viewport.h"

#include <glm/gtc/matrix_transform.hpp>
#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "Constants.h"

namespace GLBViewer
{
  void Viewport::resize(int width, int height)
  {
    mWidth = width;
    mHeight = height;
    mProjection = perspectiveProjection();
    glViewport(0, 0, width, height);
  }

  glm::vec2 Viewport::getSize() const
  {
    return glm::vec2(mWidth, mHeight);
  }

  const glm::mat4& Viewport::getProjection() const
  {
    return mProjection;
  }

  float Viewport::getAspectRatio() const
  {
    return static_cast<float>(mWidth) / mHeight;
  }

  glm::mat4 Viewport::perspectiveProjection() const
  {
    return glm::perspective(glm::radians(FOV), getAspectRatio(), NEAR_PLANE, FAR_PLANE);
  }
}
