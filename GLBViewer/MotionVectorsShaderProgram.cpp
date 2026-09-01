#include "MotionVectorsShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLBViewer
{
  MotionVectorsShaderProgram::MotionVectorsShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mModel = getUniformLocation("model");
    mPrevView = getUniformLocation("prevView");
    mCurrView = getUniformLocation("currView");
    mProjection = getUniformLocation("projection");
  }

  void MotionVectorsShaderProgram::setModel(const glm::mat4& model) const
  {
    bind();
    glUniformMatrix4fv(mModel, 1, false, glm::value_ptr(model));
  }

  void MotionVectorsShaderProgram::setPrevView(const glm::mat4& view) const
  {
    bind();
    glUniformMatrix4fv(mPrevView, 1, false, glm::value_ptr(view));
  }

  void MotionVectorsShaderProgram::setCurrView(const glm::mat4& view) const
  {
    bind();
    glUniformMatrix4fv(mCurrView, 1, false, glm::value_ptr(view));
  }

  void MotionVectorsShaderProgram::setProjection(const glm::mat4& projection) const
  {
    bind();
    glUniformMatrix4fv(mProjection, 1, false, glm::value_ptr(projection));
  }
}