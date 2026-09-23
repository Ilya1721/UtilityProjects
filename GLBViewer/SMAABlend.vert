#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 vertexUV;
out vec2 pixelCoord;
out vec4 offset0;
out vec4 offset1;
out vec4 offset2;

uniform vec2 texelSize;
uniform vec2 viewportSize;

const float SMAA_MAX_SEARCH_STEPS = 16.0;

void main()
{
  vertexUV = uv;
  pixelCoord = uv * viewportSize;
  offset0 = vec4(
    uv + texelSize * vec2(-0.25, -0.125),
    uv + texelSize * vec2( 1.25, -0.125)
  );
  offset1 = vec4(
    uv + texelSize * vec2(-0.125, -0.25),
    uv + texelSize * vec2(-0.125,  1.25)
  );
  offset2 = vec4(
    offset0.x - 2.0 * SMAA_MAX_SEARCH_STEPS * texelSize.x,
    offset0.z + 2.0 * SMAA_MAX_SEARCH_STEPS * texelSize.x,
    offset1.y - 2.0 * SMAA_MAX_SEARCH_STEPS * texelSize.y,
    offset1.w + 2.0 * SMAA_MAX_SEARCH_STEPS * texelSize.y
  );
  gl_Position = vec4(pos, 0.0, 1.0);
}