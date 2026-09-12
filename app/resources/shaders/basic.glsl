//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;        // Vertex position
layout (location = 1) in vec3 aNormal;     // Vertex normal vector
layout (location = 2) in vec2 aTexCoords;  // Vertex texture coordinates

out vec2 TexCoords;                        // Passed to fragment shader
out vec3 Normal;                           // Normal transformed to world space
out vec3 FragPos;                          // Fragment position in world space

uniform mat4 model;                        // Model matrix
uniform mat4 view;                         // Camera view matrix
uniform mat4 projection;                   // Projection matrix
uniform mat3 normalMatrix;                 // Normal transformation matrix

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));               // Compute world-space position
    Normal = normalMatrix * aNormal;                        // Compute world-space normal
    TexCoords = aTexCoords;                                 // Forward UV coordinates
    gl_Position = projection * view * vec4(FragPos, 1.0);  // Output screen clip coordinates
}

//#shader fragment
#version 330 core

out vec4 FragColor;                        // Final output color

in vec2 TexCoords;                         // Interpolated UV coordinates
in vec3 Normal;                            // Interpolated normal
in vec3 FragPos;                           // Fragment position in world space

uniform sampler2D texture_diffuse1;        // Object diffuse texture
uniform vec3 viewPos;                      // Camera position in world space
uniform bool flatShading;                  // Shading toggle: false = smooth, true = flat
uniform bool isEmissive;                   // Emissive toggle: true for glowing objects (sun)

// Lighting uniforms
uniform vec3 ambientColor;                 // Constant ambient illumination term
uniform vec3 dirLightDir;                  // Directional sunlight direction
uniform vec3 dirLightColor;                // Directional sunlight color

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb; // Sample texture color

    // Emissive objects (such as the sun) emit light directly without receiving external shading
    if (isEmissive) {
        FragColor = vec4(color, 1.0);
        return;
    }

    // Normal calculation: screen-space derivatives for flat shading, or interpolated normal for smooth shading
    vec3 norm = flatShading ? normalize(cross(dFdx(FragPos), dFdy(FragPos))) : normalize(Normal);
    if (flatShading && dot(norm, Normal) < 0.0) {
        norm = -norm;                      // Ensure flat normal points towards the front face
    }

    vec3 viewDir = normalize(viewPos - FragPos);           // Direction vector towards the camera

    // 1. Ambient lighting component
    vec3 ambient = ambientColor;

    // 2. Directional light component (parallel sunlight rays)
    vec3 dLightDir = normalize(-dirLightDir);
    float dDiff = max(dot(norm, dLightDir), 0.0);
    vec3 dDiffuse = dDiff * dirLightColor;
    vec3 dReflect = reflect(-dLightDir, norm);
    float dSpec = pow(max(dot(viewDir, dReflect), 0.0), 32);
    vec3 dSpecular = 0.5 * dSpec * dirLightColor;

    // Combine all lighting terms with the surface texture
    vec3 totalLighting = ambient + dDiffuse + dSpecular;
    FragColor = vec4(totalLighting * color, 1.0);
}
