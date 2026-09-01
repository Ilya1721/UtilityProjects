#pragma once

#include "ShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class TAAResolveShaderProgram : public ShaderProgram
  {
   public:
    TAAResolveShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );

    void setPrevFrame(const Texture2D& texture) const;
    void setCurrFrame(const Texture2D& texture) const;
    void setPrevDepthMap(const Texture2D& texture) const;
    void setCurrDepthMap(const Texture2D& texture) const;
    void setMotionVectors(const Texture2D& texture) const;
    void setIsFirstFrame(bool isFirstFrame) const;
    void setScreenSize(int width, int height) const;

   private:
    int mPrevFrame;
    int mCurrFrame;
    int mPrevDepthMap;
    int mCurrDepthMap;
    int mMotionVectors;
    int mIsFirstFrame;
    int mScreenSize;
  };
}
