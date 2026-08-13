#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;
in mat3 TBN;
out vec4 fragColor;

uniform bool hasBaseColorTexture = false;
uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

uniform bool hasNormalMap = false;
uniform sampler2D normalMap;

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

vec3 getVertexNormal()
{
  if (!hasNormalMap)
  {
    return vertexNormal;
  }
  vec3 tangentNormal = texture(normalMap, vertexUV).xyz * 2.0 - 1.0;
  return normalize(TBN * tangentNormal);
}

vec4 getDiffuse(vec4 baseColor, vec3 vertexNormal)
{
  float diffuseStrength = dot(-lightDir, vertexNormal);
  return diffuseStrength * baseColor;
}

void main()
{
  vec4 baseColor = getBaseColor();
  vec3 vertexNormal = getVertexNormal();
  vec4 diffuse = getDiffuse(baseColor, vertexNormal);
  fragColor = baseColor;
}
