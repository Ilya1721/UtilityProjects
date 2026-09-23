#pragma once

#include <glm/glm.hpp>

namespace GLBViewer
{
  inline constexpr const char* PBR_VERTEX_SHADER_PATH = "./PBR.vert";
  inline constexpr const char* PBR_FRAGMENT_SHADER_PATH = "./PBR.frag";
  inline constexpr const char* SCREEN_VERTEX_SHADER_PATH = "./Screen.vert";
  inline constexpr const char* SCREEN_FRAGMENT_SHADER_PATH = "./Screen.frag";
  inline constexpr const char* DEPTH_VERTEX_SHADER_PATH = "./Depth.vert";
  inline constexpr const char* DEPTH_FRAGMENT_SHADER_PATH = "./Depth.frag";
  inline constexpr const char* SMAA_EDGE_VERTEX_SHADER_PATH = "./SMAAEdge.vert";
  inline constexpr const char* SMAA_EDGE_FRAGMENT_SHADER_PATH = "./SMAAEdge.frag";
  inline constexpr const char* SMAA_BLEND_VERTEX_SHADER_PATH = "./SMAABlend.vert";
  inline constexpr const char* SMAA_BLEND_FRAGMENT_SHADER_PATH = "./SMAABlend.frag";
  inline constexpr const char* SMAA_NEIGHBORHOOD_VERTEX_SHADER_PATH = "./SMAANeighborhood.vert";
  inline constexpr const char* SMAA_NEIGHBORHOOD_FRAGMENT_SHADER_PATH = "./SMAANeighborhood.frag";
  inline constexpr const char* ACES_TONE_MAPPING_VERTEX_SHADER_PATH = "./ACESToneMapping.vert";
  inline constexpr const char* ACES_TONE_MAPPING_FRAGMENT_SHADER_PATH = "./ACESToneMapping.frag";
  inline constexpr const char* TAA_RESOLVE_VERTEX_SHADER_PATH = "./TAAResolve.vert";
  inline constexpr const char* TAA_RESOLVE_FRAGMENT_SHADER_PATH = "./TAAResolve.frag";
  inline constexpr const char* MOTION_VECTORS_VERTEX_SHADER_PATH = "./MotionVectors.vert";
  inline constexpr const char* MOTION_VECTORS_FRAGMENT_SHADER_PATH =
    "./MotionVectors.frag";
  inline constexpr const char* ENV_MAP_VERTEX_SHADER_PATH = "./EnvMap.vert";
  inline constexpr const char* ENV_MAP_FRAGMENT_SHADER_PATH = "./EnvMap.frag";
  inline constexpr const char* PREFILTERED_ENV_MAP_VERTEX_SHADER_PATH =
    "./PrefilteredEnvMap.vert";
  inline constexpr const char* PREFILTERED_ENV_MAP_FRAGMENT_SHADER_PATH =
    "./PrefilteredEnvMap.frag";
  inline constexpr const char* IRRADIANCE_MAP_VERTEX_SHADER_PATH = "./IrradianceMap.vert";
  inline constexpr const char* IRRADIANCE_MAP_FRAGMENT_SHADER_PATH =
    "./IrradianceMap.frag";
  inline constexpr const char* BRDF_LUT_VERTEX_SHADER_PATH = "./BRDFLUT.vert";
  inline constexpr const char* BRDF_LUT_FRAGMENT_SHADER_PATH = "./BRDFLUT.frag";
  inline constexpr const char* ENV_HDRI = "./ThirdParty/resources/hdri/beach.hdr";
  inline constexpr glm::vec4 DEFAULT_BASE_COLOR {1.0f, 1.0f, 0.0f, 1.0f};
  inline constexpr glm::vec4 BACKGROUND_COLOR { 0.741f, 0.871f, 0.871f, 1.0f };
  inline constexpr float DEPTH_TEXTURE_BORDER_COLOR[] = {1.0f, 1.0f, 1.0f, 1.0f};
  inline constexpr float ANISOTROPIC_FILTERING_LEVEL = 8.0f;
  inline constexpr float FOV = 45.0f;
  inline constexpr float NEAR_PLANE = 0.01f;
  inline constexpr float FAR_PLANE = 100.0f;
  inline constexpr float SMAA_THRESHOLD = 0.1f;
  inline constexpr float SMAA_LOCAL_CONTRAST_ADAPTION_FACTOR = 2.0f;
  inline constexpr glm::vec3 LIGHT_DIR {0.5f, -1.5f, -1.0f};
  inline constexpr int ENV_MAP_SIZE = 1024;
  inline constexpr int IRRADIANCE_MAP_SIZE = 32;
  inline constexpr int PREFILTERED_MAP_SIZE = 128;
  inline constexpr int PREFILTERED_MAP_MAX_MIP_LEVELS = 5;
  inline constexpr int BRDF_LUT_SIZE = 128;
  inline constexpr int TAA_SAMPLE_COUNT = 8;
  inline constexpr int SMAA_AREA_TEXTURE_WIDTH = 160;
  inline constexpr int SMAA_AREA_TEXTURE_HEIGHT = 560;
  inline constexpr int SMAA_SEARCH_TEXTURE_WIDTH = 64;
  inline constexpr int SMAA_SEARCH_TEXTURE_HEIGHT = 16;
}