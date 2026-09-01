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
    mModelShader = std::make_unique<PBRShaderProgram>(
      PBR_VERTEX_SHADER_PATH, PBR_FRAGMENT_SHADER_PATH
    );
    mScreenShader = std::make_unique<ScreenShaderProgram>(
      SCREEN_VERTEX_SHADER_PATH, SCREEN_FRAGMENT_SHADER_PATH
    );
    mScreenRenderBuffer = std::make_unique<ScreenRenderBuffer>();
    mScreenRenderBuffer->sendDataToGPU();
    mTAA = std::make_unique<TAA>(
      mModelShader.get(), mScene.get(), mCamera.get(), width, height
    );
    initIBL(width, height);
    mModelShader->setLightDir(glm::normalize(LIGHT_DIR));
    onViewportSizeChanged(width, height);
  }

  void Window::initIBL(int width, int height)
  {
    IBL ibl(ENV_HDRI);
    mIrradianceMap = ibl.loadIrradianceMap();
    mPrefilteredEnvMap = ibl.loadPrefilteredEnvMap();
    mBRDFLUT = ibl.loadBRDFLUT();
    mModelShader->setIrradianceMap(*mIrradianceMap);
    mModelShader->setPrefilteredEnvMap(*mPrefilteredEnvMap);
    mModelShader->setBRDFLUT(*mBRDFLUT);
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
      mModelShader->setView(mCamera->getView());
      mModelShader->setCameraPosition(mCamera->getEye());
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
    mModelShader->setView(mCamera->getView());
    mModelShader->setCameraPosition(mCamera->getEye());
  }

  void Window::onViewportSizeChanged(int width, int height)
  {
    mViewport->resize(width, height);
    mTAA->resize(width, height);
    mTAA->setProjection(mViewport->getProjection());
    mModelShader->setViewportSize(glm::vec2(width, height));
  }

  void Window::loadScene(const std::filesystem::path& scenePath)
  {
    mScene->load(scenePath);
    const auto& sceneAABB = mScene->getSceneAABB();
    auto aspectRatio = mViewport->getAspectRatio();
    mCamera->adjust(sceneAABB, aspectRatio);
    mScene->sortBlendMeshes(mCamera->getEye());
    mModelShader->setView(mCamera->getView());
    mModelShader->setCameraPosition(mCamera->getEye());
  }

  void Window::render() const
  {
    while (!glfwWindowShouldClose(mWindowHandle.get()))
    {
      const auto& frame = mTAA->render();
      mScreenShader->setScreenTexture(frame);
      mScreenShader->bind();
      mScreenRenderBuffer->bind();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glEnable(GL_FRAMEBUFFER_SRGB);
      glDisable(GL_DEPTH_TEST);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glfwSwapBuffers(mWindowHandle.get());
      glfwPollEvents();
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
