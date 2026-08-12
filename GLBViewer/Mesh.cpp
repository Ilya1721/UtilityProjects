#include "Mesh.h"

namespace GLBViewer
{
  Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const glm::mat4& transform,
    const PBRMaterial& material
  )
    : mVertices(vertices),
      mLocalTransform(transform),
      mGlobalTransform(transform),
      mMaterial(material)
  {
  }

  const glm::mat4& Mesh::getLocalTransform() const
  {
    return mLocalTransform;
  }

  const glm::mat4& Mesh::getGlobalTransform() const
  {
    return mGlobalTransform;
  }

  const PBRMaterial& Mesh::getMaterial() const
  {
    return mMaterial;
  }

  void Mesh::addChild(std::unique_ptr<Mesh> child)
  {
    mChildren.push_back(std::move(child));
  }

  void Mesh::buildHierarchy()
  {
    buildTransform(this);
    buildAABB(this);
  }

  void Mesh::buildTransform(Mesh* root)
  {
    for (auto& child : root->mChildren)
    {
      child->mGlobalTransform = root->mGlobalTransform * child->mLocalTransform;
      buildTransform(child.get());
    }
  }

  void Mesh::buildAABB(Mesh* root)
  {
    root->mAABB.apply(root->mVertices, root->mGlobalTransform);
    for (auto& child : root->mChildren)
    {
      buildAABB(child.get());
      root->mAABB.apply(child->mAABB);
    }
  }

  const AABB& Mesh::getAABB() const
  {
    return mAABB;
  }

  const std::vector<Vertex>& Mesh::getVertices() const
  {
    return mVertices;
  }

  const std::vector<std::unique_ptr<Mesh>>& Mesh::getChildren() const
  {
    return mChildren;
  }
}
