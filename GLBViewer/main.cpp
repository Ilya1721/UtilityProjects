#include <filesystem>
#include <iostream>

#include "RenderBuffer.h"
#include "Vertex.h"
#include "Window.h"

namespace fs = std::filesystem;

int main()
{
  try
  {
    auto modelPath = fs::current_path() / "ThirdParty" / "models" / "robot.glb";
    GLBViewer::Window window(1200, 800);
    window.loadScene(modelPath);
    window.render();
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
    return -1;
  }
  return 0;
}