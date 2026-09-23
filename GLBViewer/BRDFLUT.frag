#version 330 core

#include "IBLCommon.glsl"
#include "PBRCommon.glsl"

in vec2 vertexUV;
out vec2 fragColor;

vec2 integrateBRDF(float NdotV, float roughness)
{
  vec3 N = vec3(0.0, 0.0, 1.0);
  vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
  float x = 0.0;
  float y = 0.0;
  for (uint sampleIdx = 0u; sampleIdx < SAMPLE_COUNT; ++sampleIdx)
  {
    vec2 Xi = hammersley(sampleIdx, SAMPLE_COUNT);
    vec3 H = importanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(L.z, 0.0);
    float NdotH = max(H.z, 0.0);
    float VdotH = max(dot(V, H), 0.0);
    if (NdotL > 0.0)
    {
      float G = geometrySmith(N, V, L, roughness);
      float GVis = (G * VdotH) / (NdotH * NdotV);
      float Fc = pow(1.0 - VdotH, 5.0);
      x += (1.0 - Fc) * GVis;
      y += Fc * GVis;
    }
  }
  x /= float(SAMPLE_COUNT);
  y /= float(SAMPLE_COUNT);
  return vec2(x, y);
}

void main()
{
  float NdotV = vertexUV.x;
  float roughness = vertexUV.y;
  fragColor = integrateBRDF(NdotV, roughness);
}