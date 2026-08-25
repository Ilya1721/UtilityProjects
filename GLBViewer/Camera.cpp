#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/reciprocal.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Constants.h"
#include "Numeric.h"

namespace
{
  using namespace GLBViewer;
  const float FOV_Y_COTANGENT = glm::cot(glm::radians(0.5f * FOV));
  constexpr float PADDING = 1.1f;
  constexpr float EYE_TARGET_MIN_DIST_PARAM = 0.1f;
  constexpr float ZOOM_SPEED = 0.2f;

  glm::vec3 projectOnSphere(const glm::vec2& point)
  {
    glm::vec3 pointOnSphere(point, 0.0f);
    if (glm::length2(point) > 1.0f)
    {
      return glm::normalize(pointOnSphere);
    }
    auto sqrtExpr = 1.0f - point.x * point.x - point.y * point.y;
    pointOnSphere.z = std::abs(std::sqrtf(sqrtExpr));
    return pointOnSphere;
  }
}

namespace GLBViewer
{
  void Camera::adjust(const AABB& sceneAABB, float aspectRatio)
  {
    const auto& min = sceneAABB.getMin();
    const auto& max = sceneAABB.getMax();
    auto center = 0.5f * (min + max);
    auto width = max.x - center.x;
    auto height = max.y - center.y;
    auto distanceX = width * (FOV_Y_COTANGENT / aspectRatio) * PADDING;
    auto distanceY = height * FOV_Y_COTANGENT * PADDING;
    auto distance = std::max(distanceX, distanceY);
    mTarget = center;
    mEye = glm::vec3(0.0f, 0.0f, max.z + distance);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mView = glm::lookAt(mEye, mTarget, mUp);
    mEyeTargetInitialDistance = glm::length(mTarget - mEye);
  }

  void Camera::orbit(const glm::vec3& startPosNDC, const glm::vec3& endPosNDC)
  {
    auto startPosOnSphere = projectOnSphere(startPosNDC);
    auto endPosOnSphere = projectOnSphere(endPosNDC);
    auto rotationAngle = glm::angle(startPosOnSphere, endPosOnSphere);
    auto rotationAxis = glm::normalize(glm::cross(startPosOnSphere, endPosOnSphere));
    auto viewRotation = glm::mat4(glm::transpose(glm::mat3(mView)));
    rotationAxis = viewRotation * glm::vec4(rotationAxis, 0.0f);
    auto rotation = glm::rotate(-rotationAngle, rotationAxis);
    glm::vec3 eyeTargetVec = rotation * glm::vec4(mEye - mTarget, 0.0f);
    mEye = mTarget + eyeTargetVec;
    mUp = rotation * glm::vec4(mUp, 0.0f);
    mView = glm::lookAt(mEye, mTarget, mUp);
  }

  void Camera::pan(const glm::vec3& farPlaneStartPos, const glm::vec3& farPlaneEndPos)
  {
    Plane targetPlane {mTarget, mEye - mTarget};
    Ray startRay {mEye, farPlaneStartPos - mEye};
    Ray endRay {mEye, farPlaneEndPos - mEye};
    auto projectedStartPos = rayPlaneIntersection(targetPlane, startRay);
    auto projectedEndPos = rayPlaneIntersection(targetPlane, endRay);
    if (!projectedStartPos || !projectedEndPos)
    {
      return;
    }
    auto movement = projectedStartPos.value() - projectedEndPos.value();
    mEye += movement;
    mTarget += movement;
    mView = glm::lookAt(mEye, mTarget, mUp);
  }

  void Camera::zoom(float scrollSign)
  {
    auto speed = scrollSign * ZOOM_SPEED * mEyeTargetInitialDistance;
    auto eyeTargetDistance = glm::length(mTarget - mEye);
    auto speedParam = std::min(eyeTargetDistance / mEyeTargetInitialDistance, 1.0f);
    auto adjustedSpeed = speedParam * speed;
    auto minDistance = EYE_TARGET_MIN_DIST_PARAM * mEyeTargetInitialDistance;
    if (eyeTargetDistance - minDistance > adjustedSpeed)
    {
      mEye += glm::normalize(mTarget - mEye) * adjustedSpeed;
      mView = glm::lookAt(mEye, mTarget, mUp);
    }
  }

  const glm::vec3& Camera::getEye() const
  {
    return mEye;
  }

  const glm::mat4& Camera::getView() const
  {
    return mView;
  }
}
