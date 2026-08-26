#include "GLBParser.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <fastgltf/tools.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "TextureUtils.h"

namespace
{
  using namespace GLBViewer;

  int wrapGltfToOpengl(fastgltf::Wrap wrapGltf)
  {
    switch (wrapGltf)
    {
      case fastgltf::Wrap::Repeat:
        return GL_REPEAT;
      case fastgltf::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
      case fastgltf::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    }
    return GL_REPEAT;
  }

  AlphaMode alphaModeGltfToCustom(fastgltf::AlphaMode alphaMode)
  {
    switch (alphaMode)
    {
      case fastgltf::AlphaMode::Opaque:
        return AlphaMode::OPAQUE;
      case fastgltf::AlphaMode::Mask:
        return AlphaMode::MASK;
      case fastgltf::AlphaMode::Blend:
        return AlphaMode::BLEND;
    }
    return AlphaMode::OPAQUE;
  }

  int filterGltfToOpengl(const fastgltf::Optional<fastgltf::Filter>& filter)
  {
    if (!filter.has_value())
    {
      return GL_LINEAR;
    }
    switch (filter.value())
    {
      case fastgltf::Filter::Nearest:
        return GL_NEAREST;
      case fastgltf::Filter::NearestMipMapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
      case fastgltf::Filter::NearestMipMapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
      case fastgltf::Filter::Linear:
        return GL_LINEAR;
      case fastgltf::Filter::LinearMipMapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
      case fastgltf::Filter::LinearMipMapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    }
    return GL_LINEAR;
  }

  void initSamplerParameters(const fastgltf::Sampler& sampler)
  {
    auto wrapS = wrapGltfToOpengl(sampler.wrapS);
    auto wrapT = wrapGltfToOpengl(sampler.wrapT);
    auto minFilter = filterGltfToOpengl(sampler.minFilter);
    auto magFilter = filterGltfToOpengl(sampler.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    if (minFilter != GL_NEAREST && minFilter != GL_LINEAR)
    {
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  }

  std::shared_ptr<Texture2D> loadTexture(
    const RegularImage& image, const fastgltf::Sampler& sampler, bool useSRGB
  )
  {
    auto texture = std::make_shared<Texture2D>(image.width, image.height);
    texture->bind();
    auto format = getFormat(image.colorChannels);
    auto internalFormat = getInternalFormat(image.colorChannels, useSRGB);
    glTexImage2D(
      GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format,
      GL_UNSIGNED_BYTE, image.data
    );
    initSamplerParameters(sampler);
    float maxAnisotropyLevel = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropyLevel);
    float anisotropyLevel = std::min(ANISOTROPIC_FILTERING_LEVEL, maxAnisotropyLevel);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel);
    return texture;
  }
}

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

  std::unique_ptr<RegularImage> GLBParser::loadImageFromMemory(
    const fastgltf::sources::BufferView& bufferViewSource
  ) const
  {
    const auto& bufferView = mAsset->bufferViews[bufferViewSource.bufferViewIndex];
    const auto& buffer = mAsset->buffers[bufferView.bufferIndex];
    std::unique_ptr<RegularImage> image;
    std::visit(
      [&image, &bufferView](auto&& data)
      {
        using BufferDataType = std::decay_t<decltype(data)>;
        if constexpr (std::is_same_v<BufferDataType, fastgltf::sources::Array>)
        {
          auto convertedData = reinterpret_cast<const unsigned char*>(
            data.bytes.data() + bufferView.byteOffset
          );
          image = loadImage(convertedData, data.bytes.size());
        }
      },
      buffer.data
    );
    return image;
  }

  std::shared_ptr<Texture2D> GLBParser::parseTexture(
    const fastgltf::TextureInfo& textureInfo, bool useSRGB
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
    std::unique_ptr<RegularImage> image;
    std::visit(
      [this, &imageGltf, &image](auto&& data)
      {
        using ImageDataType = std::decay_t<decltype(data)>;
        if constexpr (std::is_same_v<ImageDataType, fastgltf::sources::BufferView>)
        {
          image = loadImageFromMemory(data);
        }
        else if constexpr (std::is_same_v<ImageDataType, fastgltf::sources::URI>)
        {
          image = loadImage(data.uri.c_str());
        }
      },
      imageGltf.data
    );
    const auto& samplerGltf = mAsset->samplers[textureGltf.samplerIndex.value()];
    auto texture = loadTexture(*image, samplerGltf, useSRGB);
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
    material.ior = materialGltf.ior;
    material.alphaCutoff = materialGltf.alphaCutoff;
    material.alphaMode = alphaModeGltfToCustom(materialGltf.alphaMode);
    material.emissiveStrength = materialGltf.emissiveStrength;
    material.emissiveFactor = glm::make_vec3(materialGltf.emissiveFactor.data());
    if (materialGltf.pbrData.baseColorTexture.has_value())
    {
      const auto& textureInfo = materialGltf.pbrData.baseColorTexture.value();
      material.baseColorTexture = parseTexture(textureInfo, true);
    }
    if (materialGltf.normalTexture.has_value())
    {
      material.normalMap = parseTexture(materialGltf.normalTexture.value(), false);
    }
    if (materialGltf.pbrData.metallicRoughnessTexture.has_value())
    {
      material.metallicRougnessTexture =
        parseTexture(materialGltf.pbrData.metallicRoughnessTexture.value(), false);
    }
    if (materialGltf.emissiveTexture.has_value())
    {
      material.emissiveTexture = parseTexture(materialGltf.emissiveTexture.value(), true);
    }
    if (materialGltf.transmission)
    {
      const auto& transmission = materialGltf.transmission;
      material.transmissiveFactor = transmission->transmissionFactor;
      if (transmission->transmissionTexture.has_value())
      {
        material.transmissiveTexture =
          parseTexture(transmission->transmissionTexture.value(), false);
      }
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
