#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

using StartEndPair = std::pair<glm::vec3, glm::vec3>;

namespace std
{
  template <> struct hash<StartEndPair>
  {
    size_t operator()(const StartEndPair& startEndPair) const noexcept
    {
      auto hashedStart = hash<glm::vec3> {}(startEndPair.first);
      auto hashedEnd = hash<glm::vec3> {}(startEndPair.second);
      return hashedStart ^ (hashedEnd << 1);
    }
  };
}

struct Face;

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 normal;
};

struct HalfEdge
{
  HalfEdge* next {};
  HalfEdge* prev {};
  HalfEdge* twin {};
  Vertex* startVertex {};
  Face* face {};
};

struct Face
{
  HalfEdge* halfEdge;
};

class Mesh
{
 public:
  Mesh(const std::vector<Vertex>& vertices);
  const std::vector<Vertex>& getVertices() const;
  std::vector<Vertex> fixHoles() const;

 private:
  void build();
  void createFace(size_t startVertexIdx);
  std::array<Vertex*, 3> getFaceVertices(size_t startVertexIdx);
  std::array<std::unique_ptr<HalfEdge>, 3> createFaceHalfEdges(
    const std::array<Vertex*, 3>& vertices, Face* face
  );
  void addFaceHalfEdgesStartEndPairs(
    const std::array<std::unique_ptr<HalfEdge>, 3>& faceHalfEdges
  );
  void saveFace(
    std::unique_ptr<Face>&& face, std::array<std::unique_ptr<HalfEdge>, 3>& faceHalfEdges
  );
  void connectTwins();

 private:
  std::vector<Vertex> mVertices;
  std::vector<std::unique_ptr<HalfEdge>> mHalfEdges;
  std::vector<std::unique_ptr<Face>> mFaces;
  std::unordered_map<StartEndPair, HalfEdge*> mStartEndHalfEdgeMap;
  std::vector<HalfEdge*> mTwinlessHalfEdges;
};
