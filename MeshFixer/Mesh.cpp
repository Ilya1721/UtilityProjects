#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices) : mVertices(vertices)
{
  build();
}

void Mesh::build()
{
  for (size_t vertexIdx = 0; vertexIdx < mVertices.size(); vertexIdx += 3)
  {
    createFace(vertexIdx);
  }
  connectTwins();
  fixHoles();
}

void Mesh::createFace(size_t startVertexIdx)
{
  auto face = std::make_unique<Face>();
  auto faceVertices = getFaceVertices(startVertexIdx);
  auto faceHalfEdges = createFaceHalfEdges(faceVertices, face.get());
  addFaceHalfEdgesStartEndPairs(faceHalfEdges);
  saveFace(std::move(face), faceHalfEdges);
}

void Mesh::saveFace(
  std::unique_ptr<Face>&& face, std::array<std::unique_ptr<HalfEdge>, 3>& faceHalfEdges
)
{
  mFaces.push_back(std::move(face));
  for (auto& faceHalfEdge : faceHalfEdges)
  {
    mHalfEdges.push_back(std::move(faceHalfEdge));
  }
}

void Mesh::addFaceHalfEdgesStartEndPairs(
  const std::array<std::unique_ptr<HalfEdge>, 3>& faceHalfEdges
)
{
  for (const auto& halfEdge : faceHalfEdges)
  {
    const auto& start = halfEdge->startVertex->pos;
    const auto& end = halfEdge->next->startVertex->pos;
    auto startEndPair = std::make_pair(start, end);
    auto mapValue = std::make_pair(startEndPair, halfEdge.get());
    mStartEndHalfEdgeMap.insert(mapValue);
  }
}

void Mesh::connectTwins()
{
  for (const auto& [startEndPair, halfEdge] : mStartEndHalfEdgeMap)
  {
    auto endStartPair = std::make_pair(startEndPair.second, startEndPair.first);
    auto mapIt = mStartEndHalfEdgeMap.find(endStartPair);
    if (mapIt == mStartEndHalfEdgeMap.end())
    {
      mTwinlessHalfEdges.push_back(halfEdge);
    }
    else
    {
      const auto& [twinPair, twinHalfEdge] = *mapIt;
      halfEdge->twin = twinHalfEdge;
      twinHalfEdge->twin = halfEdge;
    }
  }
}

std::array<Vertex*, 3> Mesh::getFaceVertices(size_t startVertexIdx)
{
  std::array<Vertex*, 3> vertexPointers;
  for (size_t offsetIdx = 0; offsetIdx < 3; ++offsetIdx)
  {
    vertexPointers[offsetIdx] = &mVertices[startVertexIdx + offsetIdx];
  }
  return vertexPointers;
}

std::array<std::unique_ptr<HalfEdge>, 3> Mesh::createFaceHalfEdges(
  const std::array<Vertex*, 3>& vertices, Face* face
)
{
  std::array<std::unique_ptr<HalfEdge>, 3> halfEdges;
  for (size_t idx = 0; idx < 3; ++idx)
  {
    halfEdges[idx] = std::make_unique<HalfEdge>();
    halfEdges[idx]->face = face;
  }
  for (size_t idx = 0; idx < 3; ++idx)
  {
    auto prevIdx = idx > 0 ? idx - 1 : 2;
    auto nextIdx = (idx + 1) % 3;
    halfEdges[idx]->prev = halfEdges[prevIdx].get();
    halfEdges[idx]->next = halfEdges[nextIdx].get();
    halfEdges[idx]->startVertex = vertices[idx];
  }
  return halfEdges;
}

std::vector<Vertex> Mesh::fixHoles() const
{
  std::vector<Vertex> finalVertices = mVertices;
  for (const auto& halfEdge : mTwinlessHalfEdges)
  {
    finalVertices.push_back(*halfEdge->startVertex);
  }
  return finalVertices;
}

const std::vector<Vertex>& Mesh::getVertices() const
{
  return mVertices;
}
