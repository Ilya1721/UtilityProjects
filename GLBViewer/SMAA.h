#pragma once

#include <memory>

#include "ACESToneMappingShaderProgram.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "PBRShaderProgram.h"
#include "RenderBuffer.h"
#include "SMAABlendShaderProgram.h"
#include "SMAAEdgeShaderProgram.h"
#include "SMAANeighborhoodShaderProgram.h"
#include "Scene.h"
#include "Texture2D.h"
#include "Viewport.h"

namespace GLBViewer
{
  class SMAA
  {
   public:
    SMAA(
      PBRShaderProgram* modelShader,
      Scene* scene,
      Camera* camera,
      Viewport* viewport,
      int viewportWidth,
      int viewportHeight
    );
    void resize(int viewportWidth, int viewportHeight);
    const ResizableTexture2D& render();

   private:
    void renderScreenSquare(ShaderProgram* shader) const;
    void renderToFrame() const;
    void renderToToneMappedFrame() const;
    void renderToEdgesTexture() const;
    void renderToBlendTexture() const;
    void renderToNeighborhoodTexture() const;

   private:
    PBRShaderProgram* mModelShader {};
    Scene* mScene {};
    Camera* mCamera {};
    Viewport* mViewport {};
    std::unique_ptr<ScreenRenderBuffer> mScreenRenderBuffer;
    std::unique_ptr<FrameBuffer> mScreenFrameBuffer;
    std::unique_ptr<FrameBuffer> mOpaqueFrameBuffer;
    std::unique_ptr<ResizableTexture2D> mOpaqueColor;
    std::unique_ptr<ResizableTexture2D> mFrame;
    std::unique_ptr<ResizableTexture2D> mToneMappedFrame;
    std::unique_ptr<ResizableTexture2D> mEdgesTexture;
    std::unique_ptr<ResizableTexture2D> mBlendTexture;
    std::unique_ptr<ResizableTexture2D> mNeighborhoodTexture;
    std::unique_ptr<Texture2D> mAreaTexture;
    std::unique_ptr<Texture2D> mSearchTexture;
    std::unique_ptr<SMAAEdgeShaderProgram> mEdgeShader;
    std::unique_ptr<SMAABlendShaderProgram> mBlendShader;
    std::unique_ptr<SMAANeighborhoodShaderProgram> mNeighborhoodShader;
    std::unique_ptr<ACESToneMappingShaderProgram> mToneMappingShader;
    int mViewportWidth {};
    int mViewportHeight {};
    glm::vec2 mTexelSize;
  };
}
