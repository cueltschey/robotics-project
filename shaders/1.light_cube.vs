#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Normal vector for lighting

out vec3 FragPos;  // Position of the fragment in world space
out vec3 Normal;   // Normal of the fragment

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //FragPos = vec3(model * vec4(aPos, 1.0)); // Transform vertex position to world space
    //Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space
    //gl_Position = projection * view * vec4(FragPos, 1.0);
}

