#include "Numeric.h"

#include <glm/gtc/epsilon.hpp>

namespace GLBViewer
{
  std::optional<glm::vec3> rayPlaneIntersection(const Plane& plane, const Ray& ray)
  {
    auto denominator = glm::dot(plane.normal, ray.direction);
    if (glm::epsilonEqual(denominator, 0.0f, 1e-6f))
    {
      return {};
    }
    auto numerator = glm::dot(plane.normal, plane.origin - ray.origin);
    auto t = numerator / denominator;
    if (t < 0.0f)
    {
      return {};
    }
    return ray.origin + ray.direction * t;
  }
}
