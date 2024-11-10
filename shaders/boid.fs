#version 330 core

in vec2 TexCoord; // Texture coordinates from vertex shader
out vec4 FragColor;

uniform sampler2D texture1; // The texture sampler

void main()
{
    FragColor = texture(texture1, TexCoord); // Apply the texture
}

