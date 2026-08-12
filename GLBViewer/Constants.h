#pragma once

#include <glm/glm.hpp>

namespace GLBViewer
{
  inline constexpr const char* PBR_VERTEX_SHADER_PATH = "./PBRShader.vert";
  inline constexpr const char* PBR_FRAGMENT_SHADER_PATH = "./PBRShader.frag";
  inline constexpr glm::vec4 DEFAULT_BASE_COLOR {1.0f, 1.0f, 0.0f, 1.0f};
  inline constexpr float ANISOTROPIC_FILTERING_LEVEL = 8.0f;
  inline constexpr float FOV = 45.0f;
  inline constexpr float NEAR_PLANE = 0.1f;
  inline constexpr float FAR_PLANE = 100.0f;
  inline constexpr glm::vec3 LIGHT_DIR {0.5f, -1.5f, -1.0f};
}