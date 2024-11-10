#version 330 core

out vec4 FragColor;

in vec2 TexCoords;  // Texture coordinates from vertex shader
in vec3 FragPos;    // Fragment position in world space from vertex shader
in vec3 Normal;     // Normal in world space from vertex shader

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  // Position of the light source
uniform vec3 cameraPos; // Camera position (for specular lighting)

void main()
{
    // Ambient lighting to ensure basic illumination
    vec3 ambient = 0.1 * lightColor * objectColor;
    
    // Calculate the direction of the light relative to the fragment
    vec3 lightDir = normalize(lightPos - FragPos);  // Directional light (or point light)

    // Calculate the diffuse lighting factor using the dot product (Lambert's cosine law)
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * objectColor;

    // Calculate the specular lighting (Phong model)
    vec3 viewDir = normalize(cameraPos - FragPos);  // Direction to the camera
    vec3 reflectDir = reflect(-lightDir, Normal);   // Reflect the light direction over the normal
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Specular exponent (shininess)
    vec3 specular = 0.5 * spec * lightColor; // Specular intensity scaled by light color

    // Combine the lighting components: ambient + diffuse + specular
    vec3 result = ambient + diffuse + specular;

    // Set the final color of the fragment
    FragColor = vec4(result, 1.0);
}

