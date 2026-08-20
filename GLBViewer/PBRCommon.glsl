//? #version 330 core
#ifndef PBR_COMMON
#define PBR_COMMON

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

#endif
