#include "Window.h"

#include <glfw/glfw3.h>
#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <exception>

#include "IBL.h"
#include "TextureUtils.h"

namespace
{
  using namespace GLBViewer;

  constexpr int SCREEN_FBO = 0;
  constexpr int SCREEN_MASK = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

  void onMouseMove(GLFWwindow* window, double x, double y)
  {
    auto app = static_cast<Window*>(glfwGetWindowUserPointer(window));
    app->onMouseMove(x, y);
  }

  void onButtonPressed(GLFWwindow* window, int button, int action, int mods)
  {
    auto app = static_cast<Window*>(glfwGetWindowUserPointer(window));
    app->onButtonPressed(button, action, mods);
  }

  void onScroll(GLFWwindow* window, double x, double y)
  {
    auto app = static_cast<Window*>(glfwGetWindowUserPointer(window));
    app->onScroll(x, y);
  }

  void onViewportSizeChanged(GLFWwindow* window, int width, int height)
  {
    auto app = static_cast<Window*>(glfwGetWindowUserPointer(window));
    app->onViewportSizeChanged(width, height);
  }

  void preInitSetup()
  {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  }

  void preStartSetup()
  {
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.741f, 0.871f, 0.871f, 1.0f);
  }
}

namespace GLBViewer
{
  WindowHandle::~WindowHandle()
  {
    glfwDestroyWindow(mWindowHandle);
    glfwTerminate();
  }

  GLFWwindow* WindowHandle::get() const
  {
    return mWindowHandle;
  }

  void WindowHandle::init(int width, int height)
  {
    initGLFW(width, height);
    initCallbacks();
  }

  void WindowHandle::initGLFW(int width, int height)
  {
    if (!glfwInit())
    {
      throw std::exception("glfwInit failed");
    }
    mWindowHandle = glfwCreateWindow(width, height, "GLB Viewer", nullptr, nullptr);
    if (!mWindowHandle)
    {
      throw std::exception("glfwCreateWindow failed");
    }
    glfwMakeContextCurrent(mWindowHandle);
    if (!gladLoadGL(glfwGetProcAddress))
    {
      throw std::exception("gladLoadGL failed");
    }
  }

  void WindowHandle::initCallbacks()
  {
    glfwSetCursorPosCallback(mWindowHandle, ::onMouseMove);
    glfwSetMouseButtonCallback(mWindowHandle, ::onButtonPressed);
    glfwSetScrollCallback(mWindowHandle, ::onScroll);
    glfwSetFramebufferSizeCallback(mWindowHandle, ::onViewportSizeChanged);
  }

  Window::Window(int width, int height)
  {
    mWindowHandle.init(width, height);
    glfwSetWindowUserPointer(mWindowHandle.get(), this);
    mScene = std::make_unique<Scene>();
    mViewport = std::make_unique<Viewport>();
    mCamera = std::make_unique<Camera>();
    mShader = std::make_unique<PBRShaderProgram>(
      PBR_VERTEX_SHADER_PATH, PBR_FRAGMENT_SHADER_PATH
    );
    preInitSetup();
    initIBL(width, height);
    initOffscreenRendering(width, height);
    mShader->setLightDir(glm::normalize(LIGHT_DIR));
    onViewportSizeChanged(width, height);
    preStartSetup();
  }

  void Window::initIBL(int width, int height)
  {
    IBL ibl(ENV_HDRI);
    mIrradianceMap = ibl.loadIrradianceMap();
    mPrefilteredEnvMap = ibl.loadPrefilteredEnvMap();
    mBRDFLUT = ibl.loadBRDFLUT();
    mShader->setIrradianceMap(*mIrradianceMap);
    mShader->setPrefilteredEnvMap(*mPrefilteredEnvMap);
    mShader->setBRDFLUT(*mBRDFLUT);
  }

  void Window::initOffscreenRendering(int width, int height)
  {
    mOpaqueColor = createScreenTexture(width, height);
    mOffscreenFrameBuffer = std::make_unique<FrameBuffer>();
    mOffscreenFrameBuffer->addColorAttachment(*mOpaqueColor, 0);
    mOffscreenFrameBuffer->addDepthAttachment(width, height);
  }

  void Window::onMouseMove(float cursorX, float cursorY)
  {
    auto& mvState = mCameraMovementState;
    auto cursorPos = glm::vec2(cursorX, cursorY);
    if (mvState.isPanActive)
    {
      auto startPos = screenPosToWorldPos(mvState.savedCursorPos, 1.0f);
      auto endPos = screenPosToWorldPos(cursorPos, 1.0f);
      mCamera->pan(startPos, endPos);
    }
    else if (mvState.isOrbitActive)
    {
      auto startPosNDC = screenPosToNDC(mvState.savedCursorPos);
      auto endPosNDC = screenPosToNDC(cursorPos);
      mCamera->orbit(startPosNDC, endPosNDC);
    }
    if (mvState.isOrbitActive || mvState.isPanActive)
    {
      mScene->sortBlendMeshes(mCamera->getEye());
      mShader->setView(mCamera->getView());
      mShader->setCameraPosition(mCamera->getEye());
      mvState.savedCursorPos = cursorPos;
    }
  }

  void Window::onButtonPressed(int button, int action, int mods)
  {
    double x, y;
    glfwGetCursorPos(mWindowHandle.get(), &x, &y);
    auto& mvState = mCameraMovementState;
    mvState.savedCursorPos = glm::vec2(x, y);
    mvState.isOrbitActive = action != GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT;
    mvState.isPanActive = action != GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT;
  }

  void Window::onScroll(float scrollX, float scrollY)
  {
    mCamera->zoom(scrollY);
    mShader->setView(mCamera->getView());
    mShader->setCameraPosition(mCamera->getEye());
  }

  void Window::onViewportSizeChanged(int width, int height)
  {
    mOpaqueColor->resize(width, height);
    onScreenTextureResized(*mOpaqueColor, width, height);
    mOffscreenFrameBuffer->resizeDepthAttachment(width, height);
    mViewport->resize(width, height);
    mShader->setProjection(mViewport->getProjection());
    mShader->setViewportSize(glm::vec2(width, height));
  }

  void Window::loadScene(const std::filesystem::path& scenePath)
  {
    mScene->load(scenePath);
    const auto& sceneAABB = mScene->getSceneAABB();
    auto aspectRatio = mViewport->getAspectRatio();
    mCamera->adjust(sceneAABB, aspectRatio);
    mScene->sortBlendMeshes(mCamera->getEye());
    mShader->setView(mCamera->getView());
    mShader->setCameraPosition(mCamera->getEye());
  }

  void Window::render() const
  {
    while (!glfwWindowShouldClose(mWindowHandle.get()))
    {
      mShader->bind();
      mOffscreenFrameBuffer->bind();
      glClear(SCREEN_MASK);
      mScene->renderOpaque(mShader.get());
      mOffscreenFrameBuffer->copyPixels(
        mOpaqueColor->getWidth(), mOpaqueColor->getHeight(), SCREEN_FBO, SCREEN_MASK
      );
      mShader->setOpaqueOffscreen(*mOpaqueColor);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      mScene->renderTransmissive(mShader.get());
      mScene->renderBlend(mShader.get());
      glfwSwapBuffers(mWindowHandle.get());
      glfwWaitEvents();
    }
  }

  glm::vec3 Window::screenPosToWorldPos(const glm::vec2& screenPos, float depth) const
  {
    auto viewportSize = mViewport->getSize();
    glm::vec4 viewportData = {0, 0, viewportSize};
    glm::vec3 screenPos3D(screenPos.x, viewportSize.y - screenPos.y, depth);
    return glm::unProject(
      screenPos3D, mCamera->getView(), mViewport->getProjection(), viewportData
    );
  }

  glm::vec3 Window::screenPosToNDC(const glm::vec2& screenPos) const
  {
    auto viewportSize = mViewport->getSize();
    auto flippedY = viewportSize.y - screenPos.y;
    glm::vec3 ndc {};
    ndc.x = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
    ndc.y = (2.0f * flippedY) / viewportSize.y - 1.0f;
    return ndc;
  }
}  // namespace GLBViewer
