#version 330 core

in vec2 vertexUV;
in vec2 pixelCoord;
in vec4 offset0;
in vec4 offset1;
in vec4 offset2;
out vec4 fragColor;

uniform sampler2D edgesTexture;
uniform sampler2D areaTexture;
uniform sampler2D searchTexture;
uniform vec2 texelSize;
uniform vec2 viewportSize;

const float SMAA_MAX_SEARCH_STEPS = 16.0;
const float SMAA_MAX_SEARCH_STEPS_DIAG = 8.0;
const float SMAA_CORNER_ROUNDING = 25.0;
const float SMAA_AREATEX_MAX_DISTANCE = 16.0;
const vec2 SMAA_AREATEX_PIXEL_SIZE = 1.0 / vec2(160.0, 560.0);
const float SMAA_AREATEX_SUBTEX_SIZE = 1.0 / 7.0;
const vec2 SMAA_SEARCHTEX_SIZE = vec2(66.0, 33.0);
const vec2 SMAA_SEARCHTEX_PACKED_SIZE = vec2(64.0, 16.0);
const float SMAA_AREATEX_MAX_DISTANCE_DIAG = 20.0;

float saturate(float x)
{
  return clamp(x, 0.0, 1.0);
}

vec2 saturate(vec2 x)
{
  return clamp(x, 0.0, 1.0);
}

float searchLength(vec2 e, float offset)
{
  vec2 scale = SMAA_SEARCHTEX_SIZE * vec2(0.5, -1.0);
  vec2 bias = SMAA_SEARCHTEX_SIZE * vec2(offset, 1.0);
  scale += vec2(-1.0, 1.0);
  bias += vec2(0.5, -0.5);
  scale /= SMAA_SEARCHTEX_PACKED_SIZE;
  bias /= SMAA_SEARCHTEX_PACKED_SIZE;
  vec2 coord = scale * e + bias;
  return textureLod(searchTexture, coord, 0.0).r;
}

float searchXLeft(vec2 startCoord, float endCoord)
{
  vec2 e = vec2(0.0, 1.0);
  vec2 coord = startCoord;
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS); ++i)
  {
    if (!(coord.x > endCoord && e.g > 0.8281 && e.r == 0.0))
    {
      break;
    }
    e = textureLod(edgesTexture, coord, 0.0).rg;
    coord -= vec2(2.0 * texelSize.x, 0.0);
  }
  float offset = 3.25 - (255.0 / 127.0) * searchLength(e, 0.0);
  return coord.x + texelSize.x * offset;
}

float searchXRight(vec2 startCoord, float endCoord)
{
  vec2 e = vec2(0.0, 1.0);
  vec2 coord = startCoord;
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS); ++i)
  {
    if (!(coord.x < endCoord && e.g > 0.8281 && e.r == 0.0))
    {
      break;
    }
    e = textureLod(edgesTexture, coord, 0.0).rg;
    coord += vec2(2.0 * texelSize.x, 0.0);
  }
  float offset = 3.25 - (255.0 / 127.0) * searchLength(e, 0.5);
  return coord.x - texelSize.x * offset;
}

float searchYUp(vec2 startCoord, float endCoord)
{
  vec2 e = vec2(1.0, 0.0);
  vec2 coord = startCoord;
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS); ++i)
  {
    if (!(coord.y > endCoord && e.r > 0.8281 && e.g == 0.0))
    {
      break;
    }
    e = textureLod(edgesTexture, coord, 0.0).rg;
    coord -= vec2(0.0, 2.0 * texelSize.y);
  }
  float offset = 3.25 - (255.0 / 127.0) * searchLength(e.gr, 0.0);
  return coord.y + texelSize.y * offset;
}

float searchYDown(vec2 startCoord, float endCoord)
{
  vec2 e = vec2(1.0, 0.0);
  vec2 coord = startCoord;
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS); ++i)
  {
    if (!(coord.y < endCoord && e.r > 0.8281 && e.g == 0.0))
    {
      break;
    }
    e = textureLod(edgesTexture, coord, 0.0).rg;
    coord += vec2(0.0, 2.0 * texelSize.y);
  }
  float offset = 3.25 - (255.0 / 127.0) * searchLength(e.gr, 0.5);
  return coord.y - texelSize.y * offset;
}

vec2 areaLookup(vec2 dist, float edge1, float edge2, float subpixelOffset)
{
  vec2 texcoord = SMAA_AREATEX_MAX_DISTANCE * round(4.0 * vec2(edge1, edge2)) + dist;
  texcoord = SMAA_AREATEX_PIXEL_SIZE * texcoord + 0.5 * SMAA_AREATEX_PIXEL_SIZE;
  texcoord.y += SMAA_AREATEX_SUBTEX_SIZE * subpixelOffset;
  return textureLod(areaTexture, texcoord, 0.0).rg;
}

vec2 decodeDiagBilinear(vec2 e)
{
  e.r = e.r * abs(5.0 * e.r - 3.75);
  return round(e);
}

vec2 searchDiag1(vec2 coord, vec2 direction, out vec2 edge)
{
  float dist = -1.0;
  float found = 1.0;
  vec2 current = coord;
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS_DIAG); ++i)
  {
    if (!(dist < SMAA_MAX_SEARCH_STEPS_DIAG - 1.0 && found > 0.9))
    {
      break;
    }
    current += direction * texelSize;
    dist += 1.0;
    edge = textureLod(edgesTexture, current, 0.0).rg;
    found = dot(edge, vec2(0.5));
  }

  return vec2(dist, found);
}

vec2 searchDiag2(vec2 coord, vec2 direction, out vec2 edge)
{
  float dist = -1.0;
  float found = 1.0;
  vec2 current = coord + vec2(0.25 * texelSize.x, 0.0);
  for (int i = 0; i < int(SMAA_MAX_SEARCH_STEPS_DIAG); ++i)
  {
    if (!(dist < SMAA_MAX_SEARCH_STEPS_DIAG - 1.0 && found > 0.9))
    {
      break;
    }
    current += direction * texelSize;
    dist += 1.0;
    edge = textureLod(edgesTexture, current, 0.0).rg;
    edge = decodeDiagBilinear(edge);
    found = dot(edge, vec2(0.5));
  }
  return vec2(dist, found);
}

vec2 areaLookupDiag(vec2 dist, vec2 crossingEdges, float subpixelOffset)
{
  vec2 texcoord = SMAA_AREATEX_MAX_DISTANCE_DIAG * crossingEdges + dist;
  texcoord = SMAA_AREATEX_PIXEL_SIZE * texcoord + 0.5 * SMAA_AREATEX_PIXEL_SIZE;
  texcoord.x += 0.5;
  texcoord.y += SMAA_AREATEX_SUBTEX_SIZE * subpixelOffset;
  return textureLod(areaTexture, texcoord, 0.0).rg;
}

vec2 calculateDiagWeights(vec2 texcoord, vec2 e)
{
  vec2 weights = vec2(0.0);
  vec4 d = vec4(0.0);
  vec2 endEdge;
  if (e.r > 0.0)
  {
    vec2 result = searchDiag1(texcoord, vec2(-1.0, 1.0), endEdge);
    d.x = result.x;
    d.z = result.y;
    d.x += float(endEdge.y > 0.9);
  }
  else
  {
    d.xz = vec2(0.0);
  }
  vec2 result1 = searchDiag1(texcoord, vec2(1.0, -1.0), endEdge);
  d.y = result1.x;
  d.w = result1.y;
  if (d.x + d.y > 2.0)
  {
    vec4 coords = texcoord.xyxy + texelSize.xyxy * vec4(-d.x + 0.25, d.x, d.y, -d.y - 0.25);
    vec4 c;
    c.xy = textureLod(edgesTexture, coords.xy - texelSize * vec2(1.0, 0.0), 0.0).rg;
    c.zw = textureLod(edgesTexture, coords.zw + texelSize * vec2(1.0, 0.0), 0.0).rg;
    c.yxwz = vec4(decodeDiagBilinear(c.xy), decodeDiagBilinear(c.zw));
    vec2 crossing = 2.0 * c.xz + c.yw;
    crossing *= 1.0 - step(0.9, d.zw);
    weights += areaLookupDiag(d.xy, crossing, 0.0);
  }
  vec2 result2 = searchDiag2(texcoord, vec2(-1.0, -1.0), endEdge);
  d.x = result2.x;
  d.z = result2.y;
  if (textureLod(edgesTexture, texcoord + texelSize * vec2(1.0, 0.0), 0.0).r > 0.0)
  {
    vec2 result = searchDiag2(texcoord, vec2(1.0, 1.0), endEdge);
    d.y = result.x;
    d.w = result.y;
    d.y += float(endEdge.y > 0.9);
  }
  else
  {
    d.yw = vec2(0.0);
  }
  if (d.x + d.y > 2.0)
  {
    vec4 coords = texcoord.xyxy + texelSize.xyxy * vec4(-d.x, -d.x, d.y, d.y);
    float c0 = textureLod(edgesTexture, coords.xy - texelSize * vec2(1.0, 0.0), 0.0).g;
    float c1 = textureLod(edgesTexture, coords.xy + texelSize * vec2(0.0, -1.0), 0.0).r;
    vec2 c2 = textureLod(edgesTexture, coords.zw + texelSize * vec2(1.0, 0.0), 0.0).gr;
    vec2 crossing = 2.0 * vec2(c0, c2.x) + vec2(c1, c2.y);
    crossing *= 1.0 - step(0.9, d.zw);
    weights += areaLookupDiag(d.xy, crossing, 0.0).gr;
  }
  return weights;
}

void detectHorizontalCornerPattern(inout vec2 weights, vec4 coord, vec2 d)
{
  vec2 leftRight = step(d.xy, d.yx);
  float roundingNorm = SMAA_CORNER_ROUNDING / 100.0;
  vec2 rounding = (1.0 - roundingNorm) * leftRight;
  float divisor = leftRight.x + leftRight.y;
  if (divisor > 0.0)
  {
    rounding /= divisor;
  }
  vec2 factor = vec2(1.0);
  factor.x -= rounding.x * textureLod(edgesTexture, coord.xy + texelSize * vec2(0.0, 1.0), 0.0).r;
  factor.x -= rounding.y * textureLod(edgesTexture, coord.zw + texelSize * vec2(1.0, 1.0), 0.0).r;
  factor.y -= rounding.x * textureLod(edgesTexture, coord.xy + texelSize * vec2(0.0, -2.0), 0.0).r;
  factor.y -= rounding.y * textureLod(edgesTexture, coord.zw + texelSize * vec2(1.0, -2.0), 0.0).r;
  weights *= saturate(factor);
}

void detectVerticalCornerPattern(inout vec2 weights, vec4 coord, vec2 d)
{
  vec2 leftRight = step(d.xy, d.yx);
  float roundingNorm = SMAA_CORNER_ROUNDING / 100.0;
  vec2 rounding = (1.0 - roundingNorm) * leftRight;
  float divisor = leftRight.x + leftRight.y;
  if (divisor > 0.0)
  {
    rounding /= divisor;
  }
  vec2 factor = vec2(1.0);
  factor.x -= rounding.x * textureLod(edgesTexture, coord.xy + texelSize * vec2(1.0, 0.0), 0.0).g;
  factor.x -= rounding.y * textureLod(edgesTexture, coord.zw + texelSize * vec2(1.0, 1.0), 0.0).g;
  factor.y -= rounding.x * textureLod(edgesTexture, coord.xy + texelSize * vec2(-2.0, 0.0), 0.0).g;
  factor.y -= rounding.y * textureLod(edgesTexture, coord.zw + texelSize * vec2(-2.0, 1.0), 0.0).g;
  weights *= saturate(factor);
}

void main()
{
  vec4 weights = vec4(0.0);
  vec2 e = textureLod(edgesTexture, vertexUV, 0.0).rg;
  if (e.g > 0.0)
  {
    vec2 diagWeights = calculateDiagWeights(vertexUV, e);
    weights.rg = diagWeights;
    if (weights.r + weights.g < 1e-5)
    {
      vec3 coords;
      coords.x = searchXLeft(offset0.xy, offset2.x);
      coords.y = offset1.y;
      float dLeft = coords.x;
      float e1 = textureLod(edgesTexture, coords.xy, 0.0).r;
      coords.z = searchXRight(offset0.zw, offset2.y);
      float dRight = coords.z;
      vec2 d = abs(round(vec2(dLeft, dRight) * viewportSize.x - pixelCoord.xx));
      vec2 sqrtD = sqrt(d);
      float e2 = textureLod(edgesTexture, coords.zy + vec2(texelSize.x, 0.0), 0.0).r;
      weights.rg = areaLookup(sqrtD, e1, e2, 0.0);
      coords.y = vertexUV.y;
      detectHorizontalCornerPattern(weights.rg, coords.xyzy, d);
    }
  }
  if (e.r > 0.0)
  {
    vec3 coords;
    coords.y = searchYUp(offset1.xy, offset2.z);
    coords.x = offset0.x;
    float dTop = coords.y;
    float e1 = textureLod(edgesTexture, coords.xy, 0.0).g;
    coords.z = searchYDown(offset1.zw, offset2.w);
    float dBottom = coords.z;
    vec2 d = abs(round(vec2(dTop, dBottom) * viewportSize.y - pixelCoord.yy));
    vec2 sqrtD = sqrt(d);
    float e2 = textureLod(edgesTexture, coords.xz + vec2(0.0, texelSize.y), 0.0).g;
    weights.ba = areaLookup(sqrtD, e1, e2, 0.0);
    coords.x = vertexUV.x;
    detectVerticalCornerPattern(weights.ba, coords.xyxz, d);
  }
  fragColor = weights;
}