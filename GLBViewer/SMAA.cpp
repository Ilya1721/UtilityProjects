#include "SMAA.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "TextureUtils.h"

namespace
{
  constexpr int COLOR_AND_DEPTH = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

  glm::vec4 getMetrics(int width, int height)
  {
    return glm::vec4(1.0 / width, 1.0 / height, width, height);
  }

  glm::vec2 getTexelSize(int width, int height)
  {
    return glm::vec2(1.0 / width, 1.0 / height);
  }
}

namespace GLBViewer
{
  SMAA::SMAA(
    PBRShaderProgram* modelShader,
    Scene* scene,
    Camera* camera,
    Viewport* viewport,
    int viewportWidth,
    int viewportHeight
  )
    : mModelShader(modelShader),
      mViewportWidth(viewportWidth),
      mViewportHeight(viewportHeight),
      mScene(scene),
      mCamera(camera),
      mViewport(viewport)
  {
    mTexelSize = getTexelSize(viewportWidth, viewportHeight);
    mOpaqueColor = createHDRScreenTexture(viewportWidth, viewportHeight);
    mFrame = createHDRScreenTexture(viewportWidth, viewportHeight);
    mToneMappedFrame = createLDRScreenTexture(viewportWidth, viewportHeight);
    mEdgesTexture = createRGLDRScreenTexture(viewportWidth, viewportHeight);
    mBlendTexture = createLDRScreenTexture(viewportWidth, viewportHeight);
    mNeighborhoodTexture = createHDRScreenTexture(viewportWidth, viewportHeight);
    mAreaTexture = createSMAAAreaTexture();
    mSearchTexture = createSMAASearchTexture();
    mScreenRenderBuffer = std::make_unique<ScreenRenderBuffer>();
    mScreenRenderBuffer->sendDataToGPU();
    mScreenFrameBuffer = std::make_unique<FrameBuffer>();
    mScreenFrameBuffer->addDepthAttachment(viewportWidth, viewportHeight);
    mOpaqueFrameBuffer = std::make_unique<FrameBuffer>();
    mOpaqueFrameBuffer->addColorAttachment(*mOpaqueColor, 0);
    mOpaqueFrameBuffer->addDepthAttachment(viewportWidth, viewportHeight);
    mEdgeShader = std::make_unique<SMAAEdgeShaderProgram>(
      SMAA_EDGE_VERTEX_SHADER_PATH, SMAA_EDGE_FRAGMENT_SHADER_PATH
    );
    mBlendShader = std::make_unique<SMAABlendShaderProgram>(
      SMAA_BLEND_VERTEX_SHADER_PATH, SMAA_BLEND_FRAGMENT_SHADER_PATH
    );
    mNeighborhoodShader = std::make_unique<SMAANeighborhoodShaderProgram>(
      SMAA_NEIGHBORHOOD_VERTEX_SHADER_PATH, SMAA_NEIGHBORHOOD_FRAGMENT_SHADER_PATH
    );
    mToneMappingShader = std::make_unique<ACESToneMappingShaderProgram>(
      ACES_TONE_MAPPING_VERTEX_SHADER_PATH, ACES_TONE_MAPPING_FRAGMENT_SHADER_PATH
    );
    mEdgeShader->setThreshold(SMAA_THRESHOLD);
    mEdgeShader->setLocalContrastAdaptationFactor(SMAA_LOCAL_CONTRAST_ADAPTION_FACTOR);
  }

  void SMAA::resize(int viewportWidth, int viewportHeight)
  {
    mViewportWidth = viewportWidth;
    mViewportHeight = viewportHeight;
    mTexelSize = getTexelSize(viewportWidth, viewportHeight);
    mOpaqueColor->resize(viewportWidth, viewportHeight);
    mFrame->resize(viewportWidth, viewportHeight);
    mToneMappedFrame->resize(viewportWidth, viewportHeight);
    mEdgesTexture->resize(viewportWidth, viewportHeight);
    mBlendTexture->resize(viewportWidth, viewportHeight);
    mNeighborhoodTexture->resize(viewportWidth, viewportHeight);
    mOpaqueFrameBuffer->resizeDepthAttachment(viewportWidth, viewportHeight);
    mScreenFrameBuffer->resizeDepthAttachment(viewportWidth, viewportHeight);
  }

  void SMAA::renderScreenSquare(ShaderProgram* shader) const
  {
    mScreenFrameBuffer->bind();
    mScreenRenderBuffer->bind();
    shader->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(COLOR_AND_DEPTH);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
  }

  void SMAA::renderToFrame() const
  {
    mModelShader->setProjection(mViewport->getProjection());
    mModelShader->setView(mCamera->getView());
    mScreenFrameBuffer->addColorAttachment(*mFrame, 0);
    mOpaqueFrameBuffer->bind();
    glClearColor(
      BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a
    );
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

  void SMAA::renderToToneMappedFrame() const
  {
    mScreenFrameBuffer->addColorAttachment(*mToneMappedFrame, 0);
    mToneMappingShader->setHDRTexture(*mFrame);
    renderScreenSquare(mToneMappingShader.get());
  }

  void SMAA::renderToEdgesTexture() const
  {
    mEdgeShader->setFrame(*mToneMappedFrame);
    mEdgeShader->setTexelSize(mTexelSize);
    mScreenFrameBuffer->addColorAttachment(*mEdgesTexture, 0);
    renderScreenSquare(mEdgeShader.get());
  }

  void SMAA::renderToBlendTexture() const
  {
    mBlendShader->setEdgesTexture(*mEdgesTexture);
    mBlendShader->setAreaTexture(*mAreaTexture);
    mBlendShader->setSearchTexture(*mSearchTexture);
    mBlendShader->setTexelSize(mTexelSize);
    mBlendShader->setViewportSize(glm::vec2(mViewportWidth, mViewportHeight));
    mScreenFrameBuffer->addColorAttachment(*mBlendTexture, 0);
    renderScreenSquare(mBlendShader.get());
  }

  void SMAA::renderToNeighborhoodTexture() const
  {
    mNeighborhoodShader->setBlendTexture(*mBlendTexture);
    mNeighborhoodShader->setFrame(*mToneMappedFrame);
    mNeighborhoodShader->setTexelSize(mTexelSize);
    mScreenFrameBuffer->addColorAttachment(*mNeighborhoodTexture, 0);
    renderScreenSquare(mNeighborhoodShader.get());
  }

  const ResizableTexture2D& SMAA::render()
  {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    renderToFrame();
    renderToToneMappedFrame();
    renderToEdgesTexture();
    renderToBlendTexture();
    renderToNeighborhoodTexture();
    return *mNeighborhoodTexture;
  }
}
