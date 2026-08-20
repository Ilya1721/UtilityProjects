#version 330 core

#include "Common.glsl"
#include "PBRCommon.glsl"

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

uniform sampler2D brdfLUT;
uniform samplerCube irradianceMap;
uniform samplerCube prefilteredEnvMap;

uniform vec3 lightDir;
uniform vec3 cameraPosition;

const float DIRECT_LIGHT_INTENSITY = 1.0;
const float MAX_REFLECTION_LOD = 4.0;

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

float distributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = pow(roughness, 2.0);
  float NdotH = max(dot(N, H), 0.0);
  float denominator = (pow(NdotH, 2.0) * (pow(a, 2.0) - 1.0) + 1.0);
  return pow(a, 2.0) / (PI * pow(denominator, 2.0));
}

vec3 fresnelSchlick(vec3 H, vec3 V, vec3 F0)
{
  float HdotV = max(dot(H, V), 0.0);
  return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 getIBLSpecular(vec3 N, vec3 V, vec3 F)
{
  vec3 R = reflect(-V, N);
  float NdotV = max(dot(N, V), 0.0);
  float roughness = getRoughness();
  float lod = roughness * MAX_REFLECTION_LOD;
  vec3 prefilteredColor = textureLod(prefilteredEnvMap, R, lod).rgb;
  vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
  return prefilteredColor * (F * brdf.x + brdf.y);
}

vec3 getDirectSpecular(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F)
{
  float roughness = getRoughness();
  float D = distributionGGX(N, H, roughness);
  float G = geometrySmith(N, V, L, roughness);
  vec3 numerator = D * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  return numerator / denominator;
}

vec3 getIBLDiffuse(vec3 albedo, vec3 F, vec3 N, float metallic)
{
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
  return kD * irradiance * albedo / PI;
}

vec3 getDirectDiffuse(vec3 albedo, vec3 F, vec3 N, vec3 L, float metallic)
{
  vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
  return kD * albedo / PI;
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
  vec3 iblSpecular = getIBLSpecular(N, V, F);
  vec3 directSpecular = getDirectSpecular(N, V, L, H, F);
  vec3 iblDiffuse = getIBLDiffuse(albedo, F, N, metallic);
  vec3 directDiffuse = getDirectDiffuse(albedo, F, N, L, metallic);
  vec3 directLighting = (directDiffuse + directSpecular) * NdotL * DIRECT_LIGHT_INTENSITY;
  vec3 iblLighting = iblDiffuse + iblSpecular;
  vec3 color = directLighting + iblLighting;
  fragColor = vec4(color, baseColor.a);
}
