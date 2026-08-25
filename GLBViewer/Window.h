#pragma once

#include <GLFW/glfw3.h>

#include <filesystem>
#include <memory>

#include "Camera.h"
#include "CubemapTexture.h"
#include "PBRShaderProgram.h"
#include "Scene.h"
#include "Viewport.h"
#include "FrameBuffer.h"

namespace GLBViewer
{
  struct CameraMovementState
  {
    glm::vec2 savedCursorPos {};
    bool isPanActive = false;
    bool isOrbitActive = false;
  };

  class WindowHandle
  {
   public:
    ~WindowHandle();
    GLFWwindow* get() const;
    void init(int width, int height);

   private:
    void initGLFW(int width, int height);
    void initCallbacks();

   private:
    GLFWwindow* mWindowHandle = nullptr;
  };

  class Window
  {
   public:
    Window(int width, int height);
    void loadScene(const std::filesystem::path& scenePath);
    void render() const;
    void onMouseMove(float cursorX, float cursorY);
    void onScroll(float scrollX, float scrollY);
    void onViewportSizeChanged(int width, int height);
    void onButtonPressed(int button, int action, int mods);

   private:
    glm::vec3 screenPosToWorldPos(const glm::vec2& screenPos, float depth) const;
    glm::vec3 screenPosToNDC(const glm::vec2& screenPos) const;
    void initIBL(int width, int height);
    void initOffscreenRendering(int width, int height);

   private:
    std::unique_ptr<Scene> mScene;
    std::unique_ptr<Viewport> mViewport;
    std::unique_ptr<Camera> mCamera;
    std::unique_ptr<PBRShaderProgram> mShader;
    std::unique_ptr<CubemapTexture> mIrradianceMap;
    std::unique_ptr<CubemapTexture> mPrefilteredEnvMap;
    std::unique_ptr<Texture2D> mBRDFLUT;
    std::unique_ptr<Texture2D> mOpaqueColor;
    std::unique_ptr<FrameBuffer> mOffscreenFrameBuffer;
    WindowHandle mWindowHandle;
    CameraMovementState mCameraMovementState;
  };
}  // namespace GLBViewer
