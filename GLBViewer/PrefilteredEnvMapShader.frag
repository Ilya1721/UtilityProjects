#version 330 core

#include "IBLCommon.glsl"

in vec3 modelPos;
out vec4 fragColor;

uniform samplerCube envMap;
uniform float roughness;

void main()
{
  vec3 N = normalize(modelPos);
  vec3 R = N;
  vec3 V = R;
  vec3 color = vec3(0.0);
  float totalWeight = 0.0;
  for (uint sampleIdx = 0u; sampleIdx < SAMPLE_COUNT; ++sampleIdx)
  {
    vec2 Xi = hammersley(sampleIdx, SAMPLE_COUNT);
    vec3 H = importanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL > 0.0)
    {
      color += texture(envMap, L).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  color /= totalWeight;
  fragColor = vec4(color, 1.0);
}