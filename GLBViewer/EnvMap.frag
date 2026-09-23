#version 330 core

in vec3 modelPos;
out vec4 fragColor;

uniform sampler2D equirectangularMap;

const vec2 INV_PI = vec2(0.15915494309, 0.31830988618);

vec2 sampleSphericalMap(vec3 dir)
{
  vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
  return INV_PI * uv + 0.5;
}

void main()
{
  vec3 dir = normalize(modelPos);
  vec2 uv = sampleSphericalMap(dir);
  vec3 color = texture(equirectangularMap, uv).rgb;
  fragColor = vec4(color, 1.0);
}