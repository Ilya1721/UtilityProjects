#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Vertex.h"

namespace GLBViewer
{
  class VertexBuffer
  {
   public:
    VertexBuffer();
    ~VertexBuffer();
    void bind() const;

   private:
    unsigned int mVBO {};
    unsigned int mVAO {};
  };

  class RenderBuffer
  {
   public:
    void clear();
    void loadVertices(const std::vector<Vertex>& vertices);
    void sendDataToGPU();

   private:
    void updateAlignment(const Vertex& vertex);
    void updateDataPointers(const Vertex& vertex);
    void loadRawData(size_t vertexIdx);

   private:
    int mStride = 0;
    std::vector<std::vector<const float*>> mDataPointers;
    std::vector<int> mAlignment;
    std::vector<float> mRawData;
    VertexBuffer mVertexBuffer;
  };
}
