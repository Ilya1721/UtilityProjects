#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "RenderBuffer.h"

namespace GLBViewer
{
  class PBRShaderProgram;

  class Scene
  {
   public:
    void load(const std::filesystem::path& scenePath);
    void render(PBRShaderProgram* shader) const;
    const AABB& getSceneAABB() const;

   private:
    void collectVertices(const Mesh* root, std::vector<Vertex>& vertices);

   private:
    std::unordered_map<const Mesh*, int> mMeshOffsetMap;
    std::vector<std::unique_ptr<Mesh>> mRootMeshes;
    SceneRenderBuffer mRenderBuffer;
    AABB mSceneAABB;
  };
}
