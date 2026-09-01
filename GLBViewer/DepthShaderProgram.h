#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"

namespace GLBViewer
{
  class DepthShaderProgram : public ShaderProgram
  {
   public:
    DepthShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setModel(const glm::mat4& model) const;
    void setView(const glm::mat4& view) const;
    void setProjection(const glm::mat4& projection) const;

   private:
    int mModel;
    int mView;
    int mProjection;
  };
}
