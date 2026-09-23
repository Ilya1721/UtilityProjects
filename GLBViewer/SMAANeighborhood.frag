#version 330 core

in vec2 vertexUV;
in vec4 offset;
out vec4 fragColor;

uniform sampler2D frame;
uniform sampler2D blendTexture;
uniform vec2 texelSize;

void main()
{
  vec4 a;
  a.x = textureLod(blendTexture, offset.xy, 0.0).a;
  a.y = textureLod(blendTexture, offset.zw, 0.0).g;
  vec4 currentBlend = textureLod(blendTexture, vertexUV, 0.0);
  a.z = currentBlend.b;
  a.w = currentBlend.r;
  if (dot(a, vec4(1.0)) < 1e-5)
  {
    fragColor = textureLod(frame, vertexUV, 0.0);
    return;
  }
  bool horizontal = max(a.x, a.z) > max(a.y, a.w);
  vec4 blendingOffset = vec4(0.0, a.y, 0.0, a.w);
  vec2 blendingWeight = vec2(a.y, a.w);
  if (horizontal)
  {
    blendingOffset = vec4(a.x, 0.0, a.z, 0.0);
    blendingWeight = vec2(a.x, a.z);
  }
  float weightSum = blendingWeight.x + blendingWeight.y;
  if (weightSum < 1e-5)
  {
    fragColor = textureLod(frame, vertexUV, 0.0);
    return;
  }
  blendingWeight /= weightSum;
  vec4 blendingCoord = vertexUV.xyxy + blendingOffset * vec4(texelSize.x, texelSize.y, -texelSize.x, -texelSize.y);
  vec4 color = blendingWeight.x * textureLod(frame, blendingCoord.xy, 0.0);
  color += blendingWeight.y * textureLod(frame, blendingCoord.zw, 0.0);
  fragColor = color;
}