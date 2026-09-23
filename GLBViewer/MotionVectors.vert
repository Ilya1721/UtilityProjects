#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 prevView;
uniform mat4 currView;
uniform mat4 projection;

out vec4 prevClipPos;
out vec4 currClipPos;

void main() 
{
  prevClipPos = projection * prevView * model * vec4(pos, 1.0);
  currClipPos = projection * currView * model * vec4(pos, 1.0);
  gl_Position = currClipPos;
}