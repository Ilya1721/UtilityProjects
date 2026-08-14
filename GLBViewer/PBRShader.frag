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

uniform bool hasMetallicRoughness = false;
uniform sampler2D metallicRoughness;
uniform float metallicFactor;
uniform float roughnessFactor;

uniform vec3 lightDir;
uniform vec3 cameraPosition;

const float PI = 3.14159265359;

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

float getMetallic()
{
  float metallic = 1.0;
  if (hasMetallicRoughness)
  {
    metallic = texture(metallicRoughness, vertexUV).b;
  }
  return metallicFactor * metallic;
}

float getRoughness()
{
  float roughness = 1.0;
  if (hasMetallicRoughness)
  {
    roughness = texture(metallicRoughness, vertexUV).g;
  }
  return roughnessFactor * roughness;
}

float distributionGGX(vec3 N, vec3 H, float a)
{
  float NdotH  = max(dot(N, H), 0.0);
  float denominator = (pow(NdotH, 2.0) * (pow(a, 2.0) - 1.0) + 1.0);
  return pow(a, 2.0) / (PI * pow(denominator, 2.0));
}

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

vec3 fresnelSchlick(vec3 H, vec3 V, vec3 F0)
{
  float HdotV = max(dot(H, V), 0.0);
  return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 getSpecular(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F)
{
  float roughness = getRoughness();
  float D = distributionGGX(N, H, roughness);
  float G = geometrySmith(N, V, L, roughness);
  vec3 numerator = D * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  return numerator / denominator;
}

vec3 getDiffuse(vec3 albedo, vec3 F, float metallic)
{
  vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
  return albedo * kD / PI;
}

void main()
{
  vec4 baseColor = getBaseColor();
  vec3 albedo = baseColor.rgb;
  float metallic = getMetallic();
  vec3 N = getVertexNormal();
  vec3 L = -lightDir;
  vec3 V = normalize(cameraPosition - vertexPosition);
  vec3 H = normalize(V + L);
  float NdotL = max(dot(N, L), 0.0);
  vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);
  vec3 F = fresnelSchlick(H, V, baseReflectivity);
  vec3 specular = getSpecular(N, V, L, H, F);
  vec3 diffuse = getDiffuse(albedo, F, metallic);
  vec3 reflectedRadiance = (diffuse + specular) * NdotL;
  fragColor = vec4(albedo + reflectedRadiance, baseColor.a);
}
