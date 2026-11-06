#include <filesystem>

#include "STLModel.h"

int main()
{
  auto modelsFolder = std::filesystem::current_path() / "ThirdParty" / "models";
  auto modelPath = modelsFolder / "input2.stl";
  auto modelFixedPath = modelsFolder / "output.stl";
  auto inputVertices = read(modelPath.string());
  Mesh mesh(inputVertices);
  auto outputVertices = mesh.fixHoles();
  write(outputVertices, modelFixedPath.string());
  return 0;
}