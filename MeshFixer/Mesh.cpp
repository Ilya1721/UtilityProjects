#include "Mesh.h"

#include <unordered_set>

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
  connectHolesHalfEdges();
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
      addHoleEdge(halfEdge);
    }
    else
    {
      const auto& [twinPair, twinHalfEdge] = *mapIt;
      halfEdge->twin = twinHalfEdge;
      twinHalfEdge->twin = halfEdge;
    }
  }
}

void Mesh::connectHolesHalfEdges()
{
  for (const auto& [endPos, halfEdge] : mEndPosHoleHalfEdgeMap)
  {
    const auto& startPos = halfEdge->startVertex->pos;
    auto prevHalfEdge = mEndPosHoleHalfEdgeMap[startPos];
    halfEdge->prev = prevHalfEdge;
    prevHalfEdge->next = halfEdge;
  }
}

void Mesh::addHoleEdge(HalfEdge* halfEdge)
{
  auto twinHalfEdge = std::make_unique<HalfEdge>();
  twinHalfEdge->startVertex = halfEdge->next->startVertex;
  halfEdge->twin = twinHalfEdge.get();
  twinHalfEdge->twin = halfEdge;
  auto mapValue = std::make_pair(halfEdge->startVertex->pos, twinHalfEdge.get());
  mEndPosHoleHalfEdgeMap.insert(mapValue);
  mHalfEdges.push_back(std::move(twinHalfEdge));
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
  auto holesStartHalfEdges = getHolesStartHalfEdges();
  for (const auto& halfEdge : holesStartHalfEdges)
  {
    auto startHalfEdge = halfEdge;
    auto currHalfEdge = startHalfEdge;
    do
    {
      auto holeCenter = getHoleCenter(currHalfEdge);
      auto vertices = getHoleFaceVertices(holeCenter, currHalfEdge);
      finalVertices.insert(finalVertices.end(), vertices.begin(), vertices.end());
      currHalfEdge = currHalfEdge->next;
    } while (currHalfEdge != startHalfEdge);
  }
  return finalVertices;
}

std::vector<HalfEdge*> Mesh::getHolesStartHalfEdges() const
{
  std::vector<HalfEdge*> holesStartHalfEdges;
  std::unordered_set<HalfEdge*> passedHalfEdges;
  for (const auto& [endPos, halfEdge] : mEndPosHoleHalfEdgeMap)
  {
    if (passedHalfEdges.find(halfEdge) != passedHalfEdges.end())
    {
      continue;
    }
    auto startHalfEdge = halfEdge;
    auto currHalfEdge = halfEdge;
    do
    {
      passedHalfEdges.insert(currHalfEdge);
      currHalfEdge = currHalfEdge->next;
    } while (currHalfEdge != startHalfEdge);
    holesStartHalfEdges.push_back(startHalfEdge);
  }
  return holesStartHalfEdges;
}

glm::vec3 Mesh::getHoleCenter(HalfEdge* startHalfEdge) const
{
  glm::vec3 center(0.0f);
  int halfEdgeCount = 0;
  auto currHalfEdge = startHalfEdge;
  do
  {
    center += currHalfEdge->startVertex->pos;
    ++halfEdgeCount;
    currHalfEdge = currHalfEdge->next;
  } while (currHalfEdge != startHalfEdge);
  return center / static_cast<float>(halfEdgeCount);
}

std::array<Vertex, 3> Mesh::getHoleFaceVertices(
  const glm::vec3& holeCenter, HalfEdge* holeHalfEdge
) const
{
  std::array<Vertex, 3> faceVertices;
  faceVertices[0].pos = holeCenter;
  faceVertices[1].pos = holeHalfEdge->startVertex->pos;
  faceVertices[2].pos = holeHalfEdge->next->startVertex->pos;
  auto leftEdge = holeCenter - faceVertices[1].pos;
  auto rightEdge = faceVertices[2].pos - faceVertices[1].pos;
  auto normal = glm::normalize(glm::cross(leftEdge, rightEdge));
  for (auto& vertex : faceVertices)
  {
    vertex.normal = normal;
  }
  return faceVertices;
}

const std::vector<Vertex>& Mesh::getVertices() const
{
  return mVertices;
}
