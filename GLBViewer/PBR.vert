#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureUV;
layout (location = 3) in vec4 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexUV;
out mat3 TBN;

void main()
{
    vertexUV = textureUV;
    vertexNormal = normal;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent.xyz);
    vec3 N = normalize(normalMatrix * normal);
    vec3 B = cross(N, T) * tangent.w;
    TBN = mat3(T, B, N);
    vertexPosition = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1.0);
}