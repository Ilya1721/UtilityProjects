#include "TAAResolveShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
  constexpr int PREV_FRAME_UNIT = 0;
  constexpr int CURR_FRAME_UNIT = 1;
  constexpr int PREV_DEPTH_MAP_UNIT = 2;
  constexpr int CURR_DEPTH_MAP_UNIT = 3;
  constexpr int MOTION_VECTORS_UNIT = 4;
}

namespace GLBViewer
{
  TAAResolveShaderProgram::TAAResolveShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
    : ShaderProgram(vertexShaderPath, fragmentShaderPath)
  {
    mPrevFrame = getUniformLocation("prevFrame");
    mCurrFrame = getUniformLocation("currFrame");
    mPrevDepthMap = getUniformLocation("prevDepthMap");
    mCurrDepthMap = getUniformLocation("currDepthMap");
    mMotionVectors = getUniformLocation("motionVectors");
    mIsFirstFrame = getUniformLocation("isFirstFrame");
    mScreenSize = getUniformLocation("screenSize");
  }

  void TAAResolveShaderProgram::setPrevFrame(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mPrevFrame, PREV_FRAME_UNIT);
    glBindTextureUnit(PREV_FRAME_UNIT, texture.getId());
  }

  void TAAResolveShaderProgram::setCurrFrame(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mCurrFrame, CURR_FRAME_UNIT);
    glBindTextureUnit(CURR_FRAME_UNIT, texture.getId());
  }

  void TAAResolveShaderProgram::setPrevDepthMap(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mPrevDepthMap, PREV_DEPTH_MAP_UNIT);
    glBindTextureUnit(PREV_DEPTH_MAP_UNIT, texture.getId());
  }

  void TAAResolveShaderProgram::setCurrDepthMap(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mCurrDepthMap, CURR_DEPTH_MAP_UNIT);
    glBindTextureUnit(CURR_DEPTH_MAP_UNIT, texture.getId());
  }

  void TAAResolveShaderProgram::setMotionVectors(const Texture2D& texture) const
  {
    bind();
    glUniform1i(mMotionVectors, MOTION_VECTORS_UNIT);
    glBindTextureUnit(MOTION_VECTORS_UNIT, texture.getId());
  }

  void TAAResolveShaderProgram::setIsFirstFrame(bool isFirstFrame) const
  {
    glUniform1i(mIsFirstFrame, isFirstFrame);
  }

  void TAAResolveShaderProgram::setScreenSize(int width, int height) const
  {
    glm::vec2 size(width, height);
    glUniform2fv(mScreenSize, 1, glm::value_ptr(size));
  }
}
