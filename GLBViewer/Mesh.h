#pragma once

#include <memory>
#include <vector>

#include "AABB.h"
#include "Material.h"
#include "Vertex.h"

namespace GLBViewer
{
  class Mesh
  {
   public:
    Mesh(
      const std::vector<Vertex>& vertices = {},
      const glm::mat4& transform = glm::mat4(1.0f),
      const PBRMaterial& material = {}
    );
    void addChild(std::unique_ptr<Mesh> child);
    void buildHierarchy();
    const glm::mat4& getLocalTransform() const;
    const glm::mat4& getGlobalTransform() const;
    const PBRMaterial& getMaterial() const;
    const AABB& getAABB() const;
    const std::vector<Vertex>& getVertices() const;
    const std::vector<std::unique_ptr<Mesh>>& getChildren() const;

  private:
    void buildTransform(Mesh* root);
    void buildAABB(Mesh* root);

   private:
    AABB mAABB;
    PBRMaterial mMaterial;
    glm::mat4 mLocalTransform;
    glm::mat4 mGlobalTransform;
    std::vector<Vertex> mVertices;
    std::vector<std::unique_ptr<Mesh>> mChildren;
  };
}
