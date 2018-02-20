#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 move;

void main()
{
	FragPos = vec3(move * vec4(position, 1.0));
	Normal = vec3(move * vec4(aNormal, 1.0));

    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = texCoord;
} 