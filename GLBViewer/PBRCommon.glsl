//? #version 330 core
#ifndef PBR_COMMON
#define PBR_COMMON

#include "Common.glsl"

float geometryGGX(float NdotDir, float k)
{
  return NdotDir / (NdotDir * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float a)
{
  float k = pow(a + 1.0, 2.0) / 8.0;
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx1 = geometryGGX(NdotV, k);
  float ggx2 = geometryGGX(NdotL, k);
  return ggx1 * ggx2;
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = pow(roughness, 2.0);
  float NdotH = max(dot(N, H), 0.0);
  float denominator = (pow(NdotH, 2.0) * (pow(a, 2.0) - 1.0) + 1.0);
  return pow(a, 2.0) / (PI * pow(denominator, 2.0));
}

#endif
