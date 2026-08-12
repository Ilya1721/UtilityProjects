#pragma once

#include <glm/glm.hpp>
#include <optional>

namespace GLBViewer
{
  struct Plane
  {
    glm::vec3 origin {};
    glm::vec3 normal {};
  };

  struct Ray
  {
    glm::vec3 origin {};
    glm::vec3 direction {};
  };

  std::optional<glm::vec3> rayPlaneIntersection(const Plane& plane, const Ray& ray);
}
