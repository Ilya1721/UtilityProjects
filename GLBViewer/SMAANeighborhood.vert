#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 vertexUV;
out vec4 offset;

uniform vec2 texelSize;

void main()
{
  vertexUV = uv;
  offset = vec4(uv + texelSize * vec2(1.0, 0.0), uv + texelSize * vec2(0.0, 1.0));
  gl_Position = vec4(pos, 0.0, 1.0);
}