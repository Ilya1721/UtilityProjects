#include "TAA.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "TextureUtils.h"

namespace
{
  constexpr int COLOR_AND_DEPTH = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

  float halton(int index, int base)
  {
    float f = 1.0f, r = 0.0f;
    int i = index;
    while (i > 0)
    {
      f /= base;
      r += f * (i % base);
      i /= base;
    }
    return r;
  }

  glm::vec2 getJitter(int frameIndex)
  {
    return glm::vec2(halton(frameIndex, 2), halton(frameIndex, 3)) - 0.5f;
  }

  glm::vec2 convertJitterToNDC(
    const glm::vec2& jitter, int viewportWidth, int viewportHeight
  )
  {
    return glm::vec2(
      (jitter.x * 2.0f) / static_cast<float>(viewportWidth),
      (jitter.y * 2.0f) / static_cast<float>(viewportHeight)
    );
  }
}

namespace GLBViewer
{
  TAA::TAA(
    PBRShaderProgram* modelShader,
    Scene* scene,
    Camera* camera,
    int viewportWidth,
    int viewportHeight
  )
    : mModelShader(modelShader),
      mViewportWidth(viewportWidth),
      mViewportHeight(viewportHeight),
      mScene(scene),
      mCamera(camera)
  {
    mPrevFrame = createScreenTexture(viewportWidth, viewportHeight);
    mCurrFrame = createScreenTexture(viewportWidth, viewportHeight);
    mPrevDepthMap = createDepthMap(viewportWidth, viewportHeight);
    mCurrDepthMap = createDepthMap(viewportWidth, viewportHeight);
    mMotionVectors = createMotionVectorsTexture(viewportWidth, viewportHeight);
    mOpaqueColor = createScreenTexture(viewportWidth, viewportHeight);
    mResolvedFrame = createScreenTexture(viewportWidth, viewportHeight);
    mScreenRenderBuffer = std::make_unique<ScreenRenderBuffer>();
    mScreenRenderBuffer->sendDataToGPU();
    mScreenFrameBuffer = std::make_unique<FrameBuffer>();
    mOpaqueFrameBuffer = std::make_unique<FrameBuffer>();
    mOpaqueFrameBuffer->addColorAttachment(*mOpaqueColor, 0);
    mOpaqueFrameBuffer->addDepthAttachment(viewportWidth, viewportHeight);
    mDepthShader = std::make_unique<DepthShaderProgram>(
      DEPTH_VERTEX_SHADER_PATH, DEPTH_FRAGMENT_SHADER_PATH
    );
    mMotionVectorsShader = std::make_unique<MotionVectorsShaderProgram>(
      MOTION_VECTORS_VERTEX_SHADER_PATH, MOTION_VECTORS_FRAGMENT_SHADER_PATH
    );
    mResolveShader = std::make_unique<TAAResolveShaderProgram>(
      TAA_RESOLVE_VERTEX_SHADER_PATH, TAA_RESOLVE_FRAGMENT_SHADER_PATH
    );
    mDepthPreRenderSetup = [this](const Mesh& mesh)
    { mDepthShader->setModel(mesh.getGlobalTransform()); };
    mMotionVectorsPreRenderSetup = [this](const Mesh& mesh)
    { mMotionVectorsShader->setModel(mesh.getGlobalTransform()); };
  }

  void TAA::setProjection(const glm::mat4& projection)
  {
    mProjection = projection;
  }

  void TAA::resize(int viewportWidth, int viewportHeight)
  {
    mViewportWidth = viewportWidth;
    mViewportHeight = viewportHeight;
    mPrevFrame->resize(viewportWidth, viewportHeight);
    mCurrFrame->resize(viewportWidth, viewportHeight);
    mPrevDepthMap->resize(viewportWidth, viewportHeight);
    mCurrDepthMap->resize(viewportWidth, viewportHeight);
    mMotionVectors->resize(viewportWidth, viewportHeight);
    mOpaqueColor->resize(viewportWidth, viewportHeight);
    mResolvedFrame->resize(viewportWidth, viewportHeight);
    mOpaqueFrameBuffer->resizeDepthAttachment(viewportWidth, viewportHeight);
    mScreenFrameBuffer->resizeDepthAttachment(viewportWidth, viewportHeight);
  }

  glm::mat4 TAA::getJitteredProjection() const
  {
    auto jitter = getJitter(mFrameIndex + 1);
    auto jitterNDC = convertJitterToNDC(jitter, mViewportWidth, mViewportHeight);
    glm::vec3 jitterTranslationVec {jitterNDC.x, jitterNDC.y, 0.0f};
    auto jitterTranslation = glm::translate(glm::mat4(1.0f), jitterTranslationVec);
    return jitterTranslation * mProjection;
  }

  void TAA::renderToDepthMap(const glm::mat4& view, const glm::mat4& projection) const
  {
    mDepthShader->setProjection(projection);
    mDepthShader->setView(view);
    mDepthShader->bind();
    mScreenFrameBuffer->enableColorDraw(false);
    mScreenFrameBuffer->addDepthAttachment(*mCurrDepthMap, 0);
    mScreenFrameBuffer->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    mScene->renderOpaque(mDepthPreRenderSetup);
    mScreenFrameBuffer->enableColorDraw(true);
  }

  void TAA::renderToMotionVectors(const glm::mat4& view, const glm::mat4& projection)
    const
  {
    mMotionVectorsShader->setProjection(projection);
    mMotionVectorsShader->setPrevView(mPrevView);
    mMotionVectorsShader->setCurrView(view);
    mMotionVectorsShader->bind();
    mScreenFrameBuffer->addColorAttachment(*mMotionVectors, 0);
    mScreenFrameBuffer->addDepthAttachment(mViewportWidth, mViewportHeight);
    mScreenFrameBuffer->bind();
    glClear(COLOR_AND_DEPTH);
    mScene->renderOpaque(mMotionVectorsPreRenderSetup);
  }

  void TAA::renderToFrame(const glm::mat4& view, const glm::mat4& projection) const
  {
    mModelShader->setProjection(projection);
    mScreenFrameBuffer->addColorAttachment(*mCurrFrame, 0);
    mOpaqueFrameBuffer->bind();
    glClearColor(0.741f, 0.871f, 0.871f, 1.0f);
    glClear(COLOR_AND_DEPTH);
    mScene->renderOpaque(mModelShader);
    mOpaqueFrameBuffer->copyPixels(
      mViewportWidth, mViewportHeight, *mScreenFrameBuffer, COLOR_AND_DEPTH
    );
    mModelShader->setOpaqueOffscreen(*mOpaqueColor);
    mScreenFrameBuffer->bind();
    mScene->renderTransmissive(mModelShader);
    mScene->renderBlend(mModelShader);
  }

  void TAA::resolve() const
  {
    mResolveShader->setPrevDepthMap(*mPrevDepthMap);
    mResolveShader->setCurrDepthMap(*mCurrDepthMap);
    mResolveShader->setPrevFrame(*mPrevFrame);
    mResolveShader->setCurrFrame(*mCurrFrame);
    mResolveShader->setMotionVectors(*mMotionVectors);
    mResolveShader->setScreenSize(mViewportWidth, mViewportHeight);
    mResolveShader->setIsFirstFrame(mIsFirstFrame);
    mScreenFrameBuffer->addColorAttachment(*mResolvedFrame, 0);
    mScreenFrameBuffer->bind();
    mScreenRenderBuffer->bind();
    mResolveShader->bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  const ResizableTexture2D& TAA::render()
  {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    auto jitteredProjection = getJitteredProjection();
    const auto& view = mCamera->getView();
    renderToDepthMap(view, jitteredProjection);
    renderToMotionVectors(view, jitteredProjection);
    renderToFrame(view, jitteredProjection);
    resolve();
    std::swap(mPrevDepthMap, mCurrDepthMap);
    std::swap(mPrevFrame, mResolvedFrame);
    mPrevView = view;
    mFrameIndex = (mFrameIndex + 1) % TAA_SAMPLE_COUNT;
    mIsFirstFrame = false;
    return *mPrevFrame;
  }
}
