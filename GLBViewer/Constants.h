#pragma once

#include <glm/glm.hpp>

namespace GLBViewer
{
  inline constexpr const char* PBR_VERTEX_SHADER_PATH = "./PBRShader.vert";
  inline constexpr const char* PBR_FRAGMENT_SHADER_PATH = "./PBRShader.frag";
  inline constexpr const char* ENV_MAP_VERTEX_SHADER_PATH = "./EnvMapShader.vert";
  inline constexpr const char* ENV_MAP_FRAGMENT_SHADER_PATH = "./EnvMapShader.frag";
  inline constexpr const char* PREFILTERED_ENV_MAP_VERTEX_SHADER_PATH =
    "./PrefilteredEnvMapShader.vert";
  inline constexpr const char* PREFILTERED_ENV_MAP_FRAGMENT_SHADER_PATH =
    "./PrefilteredEnvMapShader.frag";
  inline constexpr const char* IRRADIANCE_MAP_VERTEX_SHADER_PATH =
    "./IrradianceMapShader.vert";
  inline constexpr const char* IRRADIANCE_MAP_FRAGMENT_SHADER_PATH =
    "./IrradianceMapShader.frag";
  inline constexpr const char* BRDF_LUT_VERTEX_SHADER_PATH = "./BRDFLUTShader.vert";
  inline constexpr const char* BRDF_LUT_FRAGMENT_SHADER_PATH = "./BRDFLUTShader.frag";
  inline constexpr const char* ENV_HDRI = "./ThirdParty/resources/hdri/beach.hdr";
  inline constexpr glm::vec4 DEFAULT_BASE_COLOR {1.0f, 1.0f, 0.0f, 1.0f};
  inline constexpr float ANISOTROPIC_FILTERING_LEVEL = 8.0f;
  inline constexpr float FOV = 45.0f;
  inline constexpr float NEAR_PLANE = 0.1f;
  inline constexpr float FAR_PLANE = 100.0f;
  inline constexpr glm::vec3 LIGHT_DIR {0.5f, -1.5f, -1.0f};
  inline constexpr int ENV_MAP_SIZE = 1024;
  inline constexpr int IRRADIANCE_MAP_SIZE = 32;
  inline constexpr int PREFILTERED_MAP_SIZE = 128;
  inline constexpr int PREFILTERED_MAP_MAX_MIP_LEVELS = 5;
  inline constexpr int BRDF_LUT_SIZE = 128;
}