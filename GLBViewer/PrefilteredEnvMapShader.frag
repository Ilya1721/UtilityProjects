#version 330 core

#include "IBLCommon.glsl"
#include "PBRCommon.glsl"

in vec3 modelPos;
out vec4 fragColor;

uniform samplerCube envMap;
uniform float roughness;

void main()
{
  vec3 N = normalize(modelPos);
  if (roughness < 0.001)
  {
    fragColor = vec4(textureLod(envMap, N, 0.0).rgb, 1.0);
    return;
  }
  vec3 R = N;
  vec3 V = R;
  float resolution = float(textureSize(envMap, 0).x);
  float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
  vec3 color = vec3(0.0);
  float totalWeight = 0.0;
  for (uint sampleIdx = 0u; sampleIdx < SAMPLE_COUNT; ++sampleIdx)
  {
    vec2 Xi = hammersley(sampleIdx, SAMPLE_COUNT);
    vec3 H = importanceSampleGGX(Xi, N, roughness);
    float D = distributionGGX(N, H, roughness);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.001);
    float pdf = max(D * NdotH / (4.0 * HdotV), 0.0001);
    float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf);
    float mipLevel = 0.5 * log2(saSample / saTexel);
    mipLevel = max(mipLevel, 0.0);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL > 0.0)
    {
      color += textureLod(envMap, L, mipLevel).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  color /= totalWeight;
  fragColor = vec4(color, 1.0);
}