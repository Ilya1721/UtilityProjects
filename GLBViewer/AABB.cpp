#include "AABB.h"

namespace GLBViewer
{
  void AABB::apply(const std::vector<Vertex>& vertices, const glm::mat4& model)
  {
    mIsEmpty = vertices.empty();
    for (const auto& vertex : vertices)
    {
      mMin = glm::min(mMin, vertex.position);
      mMax = glm::max(mMax, vertex.position);
    }
    apply(model);
  }

  void AABB::apply(const AABB& bbox)
  {
    if (!bbox.mIsEmpty)
    {
      mMin = mIsEmpty ? bbox.mMin : glm::min(mMin, bbox.mMin);
      mMax = mIsEmpty ? bbox.mMax : glm::max(mMax, bbox.mMax);
      mIsEmpty = false;
    }
  }

  void AABB::apply(const glm::mat4& transform)
  {
    mMin = transform * glm::vec4(mMin, 1.0f);
    mMax = transform * glm::vec4(mMax, 1.0f);
  }

  const glm::vec3& AABB::getMin() const
  {
    return mMin;
  }

  const glm::vec3& AABB::getMax() const
  {
    return mMax;
  }
}