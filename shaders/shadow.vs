#version 330 core

layout (location = 0) in vec3 aPos; // Vertex position
layout (location = 1) in vec3 aNormal; // Vertex normal
layout (location = 2) in vec2 aTexCoords; // Texture coordinates

out vec2 TexCoords; // Pass texture coordinates to fragment shader
out vec3 FragPos;   // Pass fragment position to fragment shader
out vec3 Normal;    // Pass normal to fragment shader

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // Calculate the fragment position in world space by multiplying the vertex position by the model matrix
    FragPos = vec3(model * vec4(aPos, 1.0)); 
    
    // Pass the normal to the fragment shader (transformed to world space)
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    
    // Pass texture coordinates to fragment shader
    TexCoords = aTexCoords;

    // Apply model-view-projection transformation to the vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

