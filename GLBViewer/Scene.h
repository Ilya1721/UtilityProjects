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
    void renderTransmissive(const PBRShaderProgram* shader) const;
    void renderOpaque(const PBRShaderProgram* shader) const;
    void renderBlend(const PBRShaderProgram* shader) const;
    void sortBlendMeshes(const glm::vec3& cameraPos);
    const AABB& getSceneAABB() const;

   private:
    void render(const PBRShaderProgram* shader, const Mesh* mesh) const;
    void collectVertices(const Mesh* root, std::vector<Vertex>& vertices);
    void selectRenderType(const Mesh* mesh);

   private:
    std::unordered_map<const Mesh*, int> mMeshOffsetMap;
    std::vector<std::unique_ptr<Mesh>> mRootMeshes;
    std::vector<const Mesh*> mTransmissiveMeshes;
    std::vector<const Mesh*> mOpaqueMeshes;
    std::vector<const Mesh*> mBlendMeshes;
    SceneRenderBuffer mRenderBuffer;
    AABB mSceneAABB;
  };
}
