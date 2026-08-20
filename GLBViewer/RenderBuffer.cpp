#include "RenderBuffer.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <functional>

#include "StaticModels.h"

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

  void CubeRenderBuffer::sendDataToGPU()
  {
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  }

  void SquareRenderBuffer::sendDataToGPU()
  {
    bind();
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(SQUARE_VERTICES), SQUARE_VERTICES, GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  }

  void SceneRenderBuffer::updateAlignment(const Vertex& vertex)
  {
    auto updateFunc = [this](const auto& vec)
    {
      mStride += vec.length();
      mAlignment.push_back(vec.length());
    };
    updateInOrder(updateFunc, vertex);
  }

  void SceneRenderBuffer::updateDataPointers(const Vertex& vertex)
  {
    mDataPointers.emplace_back();
    auto updateFunc = [this](const auto& vec)
    { mDataPointers.back().push_back(glm::value_ptr(vec)); };
    updateInOrder(updateFunc, vertex);
  }

  void SceneRenderBuffer::loadRawData(size_t vertexIdx)
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

  void SceneRenderBuffer::clear()
  {
    mRawData.clear();
  }

  void SceneRenderBuffer::loadVertices(const std::vector<Vertex>& vertices)
  {
    updateAlignment(vertices[0]);
    for (size_t vertexIdx = 0; vertexIdx < vertices.size(); ++vertexIdx)
    {
      updateDataPointers(vertices[vertexIdx]);
      loadRawData(vertexIdx);
    }
  }

  void SceneRenderBuffer::sendDataToGPU()
  {
    bind();
    glBufferData(
      GL_ARRAY_BUFFER, mRawData.size() * sizeof(float), mRawData.data(), GL_STATIC_DRAW
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
