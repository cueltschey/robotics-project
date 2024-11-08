#version 330 core

out vec4 FragColor;

in vec2 TexCoords;  // Texture coordinates from vertex shader
in vec3 FragPos;    // Fragment position in world space from vertex shader
in vec3 Normal;     // Normal in world space from vertex shader

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  // Position of the light source

void main()
{
    // Calculate the direction of the light relative to the fragment
    vec3 lightDir = normalize(lightPos - FragPos);

    // Calculate the diffuse lighting factor using the dot product
    float diff = max(dot(Normal, lightDir) * 3.0, 0.2);
    
    // Darken the color based on the lighting direction (shadows)
    vec3 diffuse = diff * lightColor * objectColor;

    // Set the final color, darkened based on the angle to the light
    FragColor = vec4(diffuse, 1.0);
}

