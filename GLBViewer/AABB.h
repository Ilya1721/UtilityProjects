#pragma once

#include <vector>

#include "Vertex.h"

namespace GLBViewer
{
  class AABB
  {
   public:
    void apply(const std::vector<Vertex>& vertices, const glm::mat4& model);
    void apply(const AABB& bbox);
    void apply(const glm::mat4& transform);
    const glm::vec3& getMin() const;
    const glm::vec3& getMax() const;
    glm::vec3 getCenter() const;

   private:
    glm::vec3 mMin {std::numeric_limits<float>::max()};
    glm::vec3 mMax {-std::numeric_limits<float>::max()};
    bool mIsEmpty = false;
  };
}
