#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;
in mat3 TBN;
out vec4 fragColor;

uniform bool hasBaseColorTexture = false;
uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

uniform vec3 lightDir;
uniform vec3 cameraPos;

vec4 getBaseColor()
{
  vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
  if (hasBaseColorTexture)
  {
    color = texture2D(baseColorTexture, vertexUV).rgba;
  }
  return baseColorFactor * color;
}

vec4 getDiffuse(vec4 baseColor)
{
  float diffuseStrength = dot(-lightDir, vertexNormal);
  return diffuseStrength * baseColor;
}

void main()
{
  vec4 baseColor = getBaseColor();
  vec4 diffuse = getDiffuse(baseColor);
  fragColor = getDiffuse(baseColor);
}
