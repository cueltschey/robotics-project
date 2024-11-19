#version 330 core

layout (location = 0) in vec3 aPos; // Position
layout (location = 1) in vec2 aTexCoord; // Texture coordinates

out vec2 TexCoord; // Output to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
}

