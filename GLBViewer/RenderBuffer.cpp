#include "RenderBuffer.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <functional>

namespace
{
  using namespace GLBViewer;

  template <typename F> void updateInOrder(const F& updateFunc, const Vertex& vertex)
  {
    updateFunc(vertex.position);
    updateFunc(vertex.normal);
    updateFunc(vertex.texture);
    updateFunc(vertex.tangent);
  }
}

namespace GLBViewer
{
  VertexBuffer::VertexBuffer()
  {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
  }

  VertexBuffer::~VertexBuffer()
  {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
  }

  void VertexBuffer::bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBindVertexArray(mVAO);
  }

  void RenderBuffer::updateAlignment(const Vertex& vertex)
  {
    auto updateFunc = [this](const auto& vec)
    {
      mStride += vec.length();
      mAlignment.push_back(vec.length());
    };
    updateInOrder(updateFunc, vertex);
  }

  void RenderBuffer::updateDataPointers(const Vertex& vertex)
  {
    mDataPointers.emplace_back();
    auto updateFunc = [this](const auto& vec)
    { mDataPointers.back().push_back(glm::value_ptr(vec)); };
    updateInOrder(updateFunc, vertex);
  }

  void RenderBuffer::loadRawData(size_t vertexIdx)
  {
    for (int fieldIdx = 0; fieldIdx < mAlignment.size(); ++fieldIdx)
    {
      auto dataPointer = mDataPointers[vertexIdx][fieldIdx];
      for (int coordIdx = 0; coordIdx < mAlignment[fieldIdx]; ++coordIdx)
      {
        mRawData.push_back(dataPointer[coordIdx]);
      }
    }
  }

  void RenderBuffer::clear()
  {
    mRawData.clear();
  }

  void RenderBuffer::loadVertices(const std::vector<Vertex>& vertices)
  {
    updateAlignment(vertices[0]);
    for (size_t vertexIdx = 0; vertexIdx < vertices.size(); ++vertexIdx)
    {
      updateDataPointers(vertices[vertexIdx]);
      loadRawData(vertexIdx);
    }
  }

  void RenderBuffer::sendDataToGPU()
  {
    mVertexBuffer.bind();
    glBufferData(
      GL_ARRAY_BUFFER, mRawData.size() * sizeof(float), mRawData.data(), GL_DYNAMIC_DRAW
    );
    int offset = 0;
    auto stride = mStride * sizeof(float);
    for (int fieldIdx = 0; fieldIdx < mAlignment.size(); ++fieldIdx)
    {
      auto alignment = mAlignment[fieldIdx];
      auto dataPtr = reinterpret_cast<void*>(offset * sizeof(float));
      glEnableVertexAttribArray(fieldIdx);
      glVertexAttribPointer(fieldIdx, alignment, GL_FLOAT, GL_FALSE, stride, dataPtr);
      offset += alignment;
    }
  }
}
