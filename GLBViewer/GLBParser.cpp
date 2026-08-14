#include "GLBParser.h"

#include <fastgltf/tools.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "TextureUtils.h"

namespace GLBViewer
{
  std::vector<std::unique_ptr<Mesh>> GLBParser::parseScene(
    const std::filesystem::path& scenePath
  )
  {
    auto data = fastgltf::GltfDataBuffer::FromPath(scenePath);
    if (data.error() != fastgltf::Error::None)
    {
      auto excMsg = "Could not parse the file -> " + scenePath.string();
      throw std::exception(excMsg.c_str());
    }
    fastgltf::Parser parser;
    auto asset = parser.loadGltf(data.get(), scenePath.parent_path());
    mAsset = &asset.get();
    std::vector<std::unique_ptr<Mesh>> rootMeshes;
    auto sceneIdx = mAsset->defaultScene.value();
    const auto& scene = mAsset->scenes[sceneIdx];
    for (const auto& rootNodeIdx : scene.nodeIndices)
    {
      const auto& rootNode = mAsset->nodes[rootNodeIdx];
      auto rootMesh = parseNode(rootNode);
      rootMesh->buildHierarchy();
      rootMeshes.push_back(std::move(rootMesh));
    }
    return rootMeshes;
  }

  std::vector<Vertex> GLBParser::parseVertices(const fastgltf::Node& node) const
  {
    std::vector<Vertex> vertices;
    auto meshIdx = node.meshIndex.value();
    const auto& mesh = mAsset->meshes[meshIdx];
    for (const auto& primitive : mesh.primitives)
    {
      auto posAttribute = primitive.findAttribute("POSITION");
      auto normalAttribute = primitive.findAttribute("NORMAL");
      auto textureAttribute = primitive.findAttribute("TEXCOORD_0");
      auto tangentAttribute = primitive.findAttribute("TANGENT");
      auto attributesEnd = primitive.attributes.end();
      if (posAttribute == attributesEnd || normalAttribute == attributesEnd)
      {
        throw std::exception("No normal or vertex data in the mesh");
      }
      if (!primitive.indicesAccessor.has_value())
      {
        throw std::exception("Non-indexed primitives not supported");
      }
      const auto& indexAcc = mAsset->accessors[primitive.indicesAccessor.value()];
      const auto& posAcc = mAsset->accessors[posAttribute->accessorIndex];
      const auto& normalAcc = mAsset->accessors[normalAttribute->accessorIndex];
      for (size_t idx = 0; idx < indexAcc.count; ++idx)
      {
        Vertex vertex;
        auto vertexIdx = fastgltf::getAccessorElement<uint32_t>(*mAsset, indexAcc, idx);
        auto posGltf =
          fastgltf::getAccessorElement<fastgltf::math::fvec3>(*mAsset, posAcc, vertexIdx);
        vertex.position = glm::make_vec3(posGltf.data());
        auto normalGltf = fastgltf::getAccessorElement<fastgltf::math::fvec3>(
          *mAsset, normalAcc, vertexIdx
        );
        vertex.normal = glm::make_vec3(normalGltf.data());
        if (textureAttribute != attributesEnd)
        {
          const auto& textureAcc = mAsset->accessors[textureAttribute->accessorIndex];
          auto textureGltf = fastgltf::getAccessorElement<fastgltf::math::fvec2>(
            *mAsset, textureAcc, vertexIdx
          );
          vertex.texture = glm::make_vec2(textureGltf.data());
        }
        if (tangentAttribute != attributesEnd)
        {
          const auto& tangentAcc = mAsset->accessors[tangentAttribute->accessorIndex];
          auto tangentGltf = fastgltf::getAccessorElement<fastgltf::math::fvec4>(
            *mAsset, tangentAcc, vertexIdx
          );
          vertex.tangent = glm::make_vec4(tangentGltf.data());
        }
        vertices.push_back(vertex);
      }
    }
    return vertices;
  }

  std::shared_ptr<Texture2D> GLBParser::loadTextureFromMemory(
    const fastgltf::sources::BufferView& bufferViewSource, bool useGammaCorrection
  ) const
  {
    const auto& bufferView = mAsset->bufferViews[bufferViewSource.bufferViewIndex];
    const auto& buffer = mAsset->buffers[bufferView.bufferIndex];
    std::shared_ptr<Texture2D> texture;
    std::visit(
      [&texture, &bufferView, useGammaCorrection](auto&& data)
      {
        using BufferDataType = std::decay_t<decltype(data)>;
        if constexpr (std::is_same_v<BufferDataType, fastgltf::sources::Array>)
        {
          auto convertedData = reinterpret_cast<const unsigned char*>(
            data.bytes.data() + bufferView.byteOffset
          );
          auto image = loadImage(convertedData, data.bytes.size());
          texture = createImageTexture(*image, useGammaCorrection);
        }
      },
      buffer.data
    );
    return texture;
  }

  std::shared_ptr<Texture2D> GLBParser::loadTextureFromFile(
    const char* filePath, bool useGammaCorrection
  ) const
  {
    auto image = loadImage(filePath);
    return createImageTexture(*image, useGammaCorrection);
  }

  std::shared_ptr<Texture2D> GLBParser::getTexture(
    const fastgltf::TextureInfo& textureInfo, bool useGammaCorrection
  ) const
  {
    auto textureIndex = textureInfo.textureIndex;
    auto textureMapIt = mTextureMap.find(textureIndex);
    if (textureMapIt != mTextureMap.end())
    {
      return textureMapIt->second;
    }
    const auto& textureGltf = mAsset->textures[textureIndex];
    const auto& imageGltf = mAsset->images[textureGltf.imageIndex.value()];
    std::shared_ptr<Texture2D> texture;
    std::visit(
      [this, &imageGltf, &texture, useGammaCorrection](auto&& data)
      {
        using ImageDataType = std::decay_t<decltype(data)>;
        if constexpr (std::is_same_v<ImageDataType, fastgltf::sources::BufferView>)
        {
          texture = loadTextureFromMemory(data, useGammaCorrection);
        }
        else if constexpr (std::is_same_v<ImageDataType, fastgltf::sources::URI>)
        {
          texture = loadTextureFromFile(data.uri.c_str(), useGammaCorrection);
        }
      },
      imageGltf.data
    );
    mTextureMap.insert({textureIndex, texture});
    return texture;
  }

  PBRMaterial GLBParser::parseMaterial(const fastgltf::Node& node) const
  {
    auto meshId = node.meshIndex.value();
    const auto& mesh = mAsset->meshes[meshId];
    const auto& primitive = mesh.primitives[0];
    if (!primitive.materialIndex.has_value())
    {
      return {};
    }
    const auto& materialGltf = mAsset->materials[primitive.materialIndex.value()];
    PBRMaterial material;
    material.baseColorFactor =
      glm::make_vec4(materialGltf.pbrData.baseColorFactor.data());
    material.metallicFactor = materialGltf.pbrData.metallicFactor;
    material.rougnessFactor = materialGltf.pbrData.roughnessFactor;
    if (materialGltf.pbrData.baseColorTexture.has_value())
    {
      const auto& textureInfo = materialGltf.pbrData.baseColorTexture.value();
      material.baseColorTexture = getTexture(textureInfo, true);
    }
    if (materialGltf.normalTexture.has_value())
    {
      material.normalMap = getTexture(materialGltf.normalTexture.value(), false);
    }
    if (materialGltf.pbrData.metallicRoughnessTexture.has_value())
    {
      material.metallicRougnessTexture =
        getTexture(materialGltf.pbrData.metallicRoughnessTexture.value(), false);
    }
    return material;
  }

  std::unique_ptr<Mesh> GLBParser::parseMesh(const fastgltf::Node& node) const
  {
    auto transformGltf = fastgltf::getTransformMatrix(node);
    auto transform = glm::make_mat4(transformGltf.data());
    if (!node.meshIndex.has_value())
    {
      return std::make_unique<Mesh>(std::vector<Vertex> {}, transform);
    }
    auto vertices = parseVertices(node);
    auto material = parseMaterial(node);
    return std::make_unique<Mesh>(vertices, transform, material);
  }

  std::unique_ptr<Mesh> GLBParser::parseNode(const fastgltf::Node& node) const
  {
    auto mesh = parseMesh(node);
    for (const auto& childIdx : node.children)
    {
      const auto& childNode = mAsset->nodes[childIdx];
      mesh->addChild(parseNode(childNode));
    }
    return mesh;
  }
}
