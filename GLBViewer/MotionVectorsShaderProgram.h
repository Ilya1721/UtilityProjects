#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

namespace GLBViewer
{
  class MotionVectorsShaderProgram : public ShaderProgram
  {
   public:
    MotionVectorsShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setModel(const glm::mat4& model) const;
    void setPrevView(const glm::mat4& view) const;
    void setCurrView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;

   private:
    int mModel;
    int mPrevView;
    int mCurrView;
    int mProjection;
  };
}
