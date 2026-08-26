#version 330 core

#include "PBRCommon.glsl"

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;
in mat3 TBN;
out vec4 fragColor;

uniform bool hasBaseColorTexture = false;
uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

uniform bool hasTransmissionTexture = false;
uniform sampler2D transmissionTexture;
uniform float transmissionFactor;
uniform float ior;

const int ALPHA_MODE_OPAQUE = 0;
const int ALPHA_MODE_BLEND = 1;
const int ALPHA_MODE_MASK = 2;
uniform int alphaMode;
uniform float alphaCutoff;

uniform bool hasNormalMap = false;
uniform sampler2D normalMap;

uniform bool hasMetallicRoughness = false;
uniform sampler2D metallicRoughness;
uniform float metallicFactor;
uniform float roughnessFactor;

uniform sampler2D brdfLUT;
uniform samplerCube irradianceMap;
uniform samplerCube prefilteredEnvMap;

uniform sampler2D opaqueOffscreen;

uniform vec3 lightDir;
uniform vec3 cameraPosition;
uniform vec2 viewportSize;

const float DIRECT_LIGHT_INTENSITY = 2.5;
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

float getTransmission()
{
  float transmission = 1.0;
  if (hasTransmissionTexture)
  {
    transmission = texture(transmissionTexture, vertexUV).r;
  }
  return transmissionFactor * transmission;
}

vec3 directFresnelSchlick(vec3 H, vec3 V, vec3 F0)
{
  float HdotV = max(dot(H, V), 0.0);
  return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 iblFresnelSchlick(vec3 N, vec3 V, vec3 F0, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - NdotV, 5.0);
}

vec3 getIBLSpecular(vec3 N, vec3 V, vec3 F, float roughness)
{
  vec3 R = reflect(-V, N);
  float NdotV = max(dot(N, V), 0.0);
  float lod = roughness * MAX_REFLECTION_LOD;
  vec3 prefilteredColor = textureLod(prefilteredEnvMap, R, lod).rgb;
  vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
  return prefilteredColor * (F * brdf.x + brdf.y);
}

vec3 getDirectSpecular(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F, float roughness)
{
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

vec3 getTransmissionAffectedColor(vec3 surfaceColor, vec3 H, vec3 V)
{
  float transmission = getTransmission();
  if (transmission < 1e-4 || alphaMode == ALPHA_MODE_BLEND)
  {
    return surfaceColor;
  }
  vec2 screenUV = gl_FragCoord.xy / viewportSize;
  vec3 transmittedColor = texture(opaqueOffscreen, screenUV).rgb;
  float F0 = pow((ior - 1.0) / (ior + 1.0), 2.0);
  vec3 F = directFresnelSchlick(H, V, vec3(F0));
  vec3 transmissionWeight = transmission * (1.0 - F);
  return surfaceColor + transmittedColor * transmissionWeight;
}

void main()
{
  vec4 baseColor = getBaseColor();
  if (alphaMode == ALPHA_MODE_MASK && baseColor.a < alphaCutoff)
  {
    discard;
  }
  vec3 albedo = baseColor.rgb;
  float metallic = getMetallic();
  float roughness = getRoughness();
  vec3 N = getVertexNormal();
  vec3 L = -lightDir;
  vec3 V = normalize(cameraPosition - vertexPosition);
  vec3 H = normalize(V + L);
  float NdotL = max(dot(N, L), 0.0);
  vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);
  vec3 iblF = iblFresnelSchlick(N, V, baseReflectivity, roughness);
  vec3 iblSpecular = getIBLSpecular(N, V, iblF, roughness);
  vec3 iblDiffuse = getIBLDiffuse(albedo, iblF, N, metallic);
  vec3 directF = directFresnelSchlick(H, V, baseReflectivity);
  vec3 directSpecular = getDirectSpecular(N, V, L, H, directF, roughness);
  vec3 directDiffuse = getDirectDiffuse(albedo, directF, N, L, metallic);
  vec3 directLighting = (directDiffuse + directSpecular) * NdotL * DIRECT_LIGHT_INTENSITY;
  vec3 iblLighting = iblDiffuse + iblSpecular;
  vec3 surfaceColor = directLighting + iblLighting;
  vec3 color = getTransmissionAffectedColor(surfaceColor, H, V);
  fragColor = vec4(color, baseColor.a);
}
