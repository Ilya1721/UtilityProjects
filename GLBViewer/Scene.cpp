#include "Scene.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "GLBParser.h"
#include "PBRShaderProgram.h"

namespace GLBViewer
{
  void Scene::load(const std::filesystem::path& scenePath)
  {
    mRenderBuffer.clear();
    GLBParser parser;
    mRootMeshes = parser.parseScene(scenePath);
    std::vector<Vertex> vertices;
    for (const auto& rootMesh : mRootMeshes)
    {
      mSceneAABB.apply(rootMesh->getAABB());
      collectVertices(rootMesh.get(), vertices);
    }
    mRenderBuffer.loadVertices(vertices);
    mRenderBuffer.sendDataToGPU();
  }

  const AABB& Scene::getSceneAABB() const
  {
    return mSceneAABB;
  }

  void Scene::render(PBRShaderProgram* shader) const
  {
    mRenderBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const auto& [mesh, vertexOffset] : mMeshOffsetMap)
    {
      shader->setModel(mesh->getGlobalTransform());
      shader->setMaterial(mesh->getMaterial());
      glDrawArrays(GL_TRIANGLES, vertexOffset, mesh->getVertices().size());
    }
  }

  void Scene::collectVertices(const Mesh* root, std::vector<Vertex>& vertices)
  {
    if (!root)
    {
      return;
    }
    const auto& rootVertices = root->getVertices();
    if (!rootVertices.empty())
    {
      mMeshOffsetMap.insert({root, vertices.size()});
      vertices.insert(vertices.end(), rootVertices.begin(), rootVertices.end());
    }
    for (const auto& child : root->getChildren())
    {
      collectVertices(child.get(), vertices);
    }
  }
}
