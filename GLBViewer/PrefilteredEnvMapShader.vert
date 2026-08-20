#version 330 core

layout (location = 0) in vec3 pos;

out vec3 modelPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
  modelPos = pos;
  gl_Position = projection * view * vec4(pos, 1.0);
}