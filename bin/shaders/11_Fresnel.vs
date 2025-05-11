#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 WorldPos;
out vec3 WorldNormal;
out vec3 viewDir;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    WorldPos = worldPosition.xyz;

    WorldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoords = aTexCoords;

    viewDir = normalize(cameraPosition - WorldPos);

    gl_Position = projection * view * worldPosition;
}
