#include "Scene.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "GLBParser.h"
#include "PBRShaderProgram.h"

namespace
{
  using namespace GLBViewer;

  float getMeshCameraDistance(const Mesh* mesh, const glm::vec3& cameraPos)
  {
    auto meshCenter = mesh->getAABB().getCenter();
    glm::vec3 meshCenterWS = mesh->getGlobalTransform() * glm::vec4(meshCenter, 1.0f);
    return glm::length(cameraPos - meshCenterWS);
  }
}

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

  void Scene::render(const PBRShaderProgram* shader, const Mesh* mesh) const
  {
    shader->setModel(mesh->getGlobalTransform());
    shader->setMaterial(mesh->getMaterial());
    auto vertexOffset = mMeshOffsetMap.at(mesh);
    glDrawArrays(GL_TRIANGLES, vertexOffset, mesh->getVertices().size());
  }

  void Scene::renderTransmissive(const PBRShaderProgram* shader) const
  {
    mRenderBuffer.bind();
    glDepthMask(GL_FALSE);
    for (const auto& mesh : mTransmissiveMeshes)
    {
      render(shader, mesh);
    }
    glDepthMask(GL_TRUE);
  }

  void Scene::renderOpaque(const PBRShaderProgram* shader) const
  {
    mRenderBuffer.bind();
    for (const auto& mesh : mOpaqueMeshes)
    {
      render(shader, mesh);
    }
  }

  void Scene::renderBlend(const PBRShaderProgram* shader) const
  {
    mRenderBuffer.bind();
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (const auto& mesh : mBlendMeshes)
    {
      render(shader, mesh);
    }
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
  }

  void Scene::sortBlendMeshes(const glm::vec3& cameraPos)
  {
    std::sort(
      mBlendMeshes.begin(),
      mBlendMeshes.end(),
      [&cameraPos](const Mesh* left, const Mesh* right)
      {
        auto leftCameraDist = getMeshCameraDistance(left, cameraPos);
        auto rightCameraDist = getMeshCameraDistance(right, cameraPos);
        return leftCameraDist > rightCameraDist;
      }
    );
  }

  void Scene::selectRenderType(const Mesh* mesh)
  {
    const auto& material = mesh->getMaterial();
    if (material.alphaMode == AlphaMode::BLEND)
    {
      mBlendMeshes.push_back(mesh);
    }
    else if (material.transmissionFactor > 0.0f)
    {
      mTransmissiveMeshes.push_back(mesh);
    }
    else
    {
      mOpaqueMeshes.push_back(mesh);
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
      selectRenderType(root);
      mMeshOffsetMap.insert({root, vertices.size()});
      vertices.insert(vertices.end(), rootVertices.begin(), rootVertices.end());
    }
    for (const auto& child : root->getChildren())
    {
      collectVertices(child.get(), vertices);
    }
  }
}
