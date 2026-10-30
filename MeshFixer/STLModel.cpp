#include "STLModel.h"

#include <fstream>

using FileIterator = std::istreambuf_iterator<char>;

glm::vec3 readXYZ(const char*& pointer)
{
  glm::vec3 xyz;
  for (int coordIdx = 0; coordIdx < 3; ++coordIdx)
  {
    xyz[coordIdx] = *reinterpret_cast<const float*>(pointer);
    pointer += sizeof(float);
  }
  return xyz;
}

void writeXYZ(std::string& buffer, const glm::vec3& xyz)
{
  for (int coordIdx = 0; coordIdx < 3; ++coordIdx)
  {
    buffer.append(reinterpret_cast<const char*>(&xyz[coordIdx]), sizeof(float));
  }
}

std::vector<Vertex> read(const std::string& path)
{
  std::ifstream file(path);
  std::string content {FileIterator(file), FileIterator()};
  auto pointer = content.c_str() + 80;
  auto trianglesCount = *reinterpret_cast<const uint32_t*>(pointer);
  pointer += sizeof(uint32_t);
  std::vector<Vertex> vertices;
  for (size_t faceIdx = 0; faceIdx < trianglesCount; ++faceIdx)
  {
    auto normal = readXYZ(pointer);
    for (size_t vertexIdx = 0; vertexIdx < 3; ++vertexIdx)
    {
      auto pos = readXYZ(pointer);
      vertices.emplace_back(pos, normal);
    }
    pointer += sizeof(uint16_t);
  }
  return vertices;
}

void write(const std::vector<Vertex>& vertices, const std::string& path)
{
  std::string content(80, ' ');
  uint32_t trianglesCount = vertices.size() / 3;
  content.append(reinterpret_cast<const char*>(&trianglesCount), sizeof(uint32_t));
  for (size_t faceIdx = 0; faceIdx < trianglesCount; ++faceIdx)
  {
    auto startVertexIdx = 3 * faceIdx;
    writeXYZ(content, vertices[startVertexIdx].normal);
    for (size_t vertexIdx = 0; vertexIdx < 3; ++vertexIdx)
    {
      writeXYZ(content, vertices[startVertexIdx + vertexIdx].pos);
    }
    content.append("", sizeof(uint16_t));
  }
  std::ofstream file(path);
  file << content;
}