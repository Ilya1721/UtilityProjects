#pragma once

#include <glm/glm.hpp>

namespace GLBViewer
{
  struct Vertex
  {
    glm::vec3 position {};
    glm::vec3 normal {};
    glm::vec2 texture {};
    glm::vec4 tangent {};
  };
}
