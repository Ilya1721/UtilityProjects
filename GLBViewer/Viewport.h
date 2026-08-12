#pragma once

#include <glm/glm.hpp>

namespace GLBViewer
{
  class Viewport
  {
  public:
    void resize(int width, int height);
    const glm::mat4& getProjection() const;
    glm::vec2 getSize() const;
    float getAspectRatio() const;

  private:
    glm::mat4 perspectiveProjection() const;

  private:
    int mWidth;
    int mHeight;
    glm::mat4 mProjection;
  };
}

