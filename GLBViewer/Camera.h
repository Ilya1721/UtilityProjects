#pragma once

#include <glm/glm.hpp>

#include "AABB.h"

namespace GLBViewer
{
  class Camera
  {
   public:
    void adjust(const AABB& sceneAABB, float aspectRatio);
    void zoom(float scrollSign);
    void pan(const glm::vec3& farPlaneStartPos, const glm::vec3& farPlaneEndPos);
    void orbit(const glm::vec3& startPosNDC, const glm::vec3& endPosNDC);
    const glm::vec3& getEye() const;
    const glm::mat4& getView() const;

   private:
    glm::vec3 mEye;
    glm::vec3 mTarget;
    glm::vec3 mUp;
    glm::mat4 mView;
    float mEyeTargetInitialDistance;
  };
}
