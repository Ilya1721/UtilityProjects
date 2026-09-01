#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
#include "DepthShaderProgram.h"
#include "FrameBuffer.h"
#include "MotionVectorsShaderProgram.h"
#include "PBRShaderProgram.h"
#include "RenderBuffer.h"
#include "Scene.h"
#include "TAAResolveShaderProgram.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class TAA
  {
   public:
    TAA(
      PBRShaderProgram* modelShader,
      Scene* scene,
      Camera* camera,
      int viewportWidth,
      int viewportHeight
    );
    void setProjection(const glm::mat4& projection);
    void resize(int viewportWidth, int viewportHeight);
    const ResizableTexture2D& render();

   private:
    void renderToDepthMap(const glm::mat4& view, const glm::mat4& projection) const;
    void renderToMotionVectors(const glm::mat4& view, const glm::mat4& projection) const;
    void renderToFrame(const glm::mat4& view, const glm::mat4& projection) const;
    void resolve() const;
    glm::mat4 getJitteredProjection() const;

   private:
    PBRShaderProgram* mModelShader {};
    Scene* mScene {};
    Camera* mCamera {};
    glm::mat4 mPrevView;
    glm::mat4 mProjection;
    PreRenderSetup mDepthPreRenderSetup;
    PreRenderSetup mMotionVectorsPreRenderSetup;
    std::unique_ptr<ResizableTexture2D> mPrevFrame;
    std::unique_ptr<ResizableTexture2D> mCurrFrame;
    std::unique_ptr<ResizableTexture2D> mPrevDepthMap;
    std::unique_ptr<ResizableTexture2D> mCurrDepthMap;
    std::unique_ptr<ResizableTexture2D> mMotionVectors;
    std::unique_ptr<ResizableTexture2D> mOpaqueColor;
    std::unique_ptr<ResizableTexture2D> mResolvedFrame;
    std::unique_ptr<ScreenRenderBuffer> mScreenRenderBuffer;
    std::unique_ptr<FrameBuffer> mScreenFrameBuffer;
    std::unique_ptr<FrameBuffer> mOpaqueFrameBuffer;
    std::unique_ptr<DepthShaderProgram> mDepthShader;
    std::unique_ptr<MotionVectorsShaderProgram> mMotionVectorsShader;
    std::unique_ptr<TAAResolveShaderProgram> mResolveShader;
    int mFrameIndex {};
    int mViewportWidth {};
    int mViewportHeight {};
    bool mIsFirstFrame = true;
  };
}
