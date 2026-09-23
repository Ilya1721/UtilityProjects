#version 330 core

#include "Common.glsl"

in vec3 modelPos;
out vec4 fragColor;

uniform samplerCube envMap;

void main()
{
  vec3 N = normalize(modelPos);
  vec3 tempUp = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(tempUp, N));
  vec3 up = normalize(cross(N, right));
  vec3 irradiance = vec3(0.0);
  float sampleDelta = 0.025;
  float sampleCount = 0.0;
  for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
  {
    for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    {
      vec3 point = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
      vec3 dir = point.x * right + point.y * up + point.z * N;
      irradiance += texture(envMap, dir).rgb * cos(theta) * sin(theta);
      sampleCount += 1.0;
    }
  }
  irradiance = PI * irradiance / sampleCount;
  fragColor = vec4(irradiance, 1.0);
}