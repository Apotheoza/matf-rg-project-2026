//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform bool flatShading;
uniform bool isEmissive;

uniform vec3 ambientColor;

uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightColor;
uniform float spotCutOff;
uniform float spotOuterCutOff;

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    if (isEmissive) {
        FragColor = vec4(color, 1.0);
        return;
    }

    vec3 norm = flatShading ? normalize(cross(dFdx(FragPos), dFdy(FragPos))) : normalize(Normal);
    if (flatShading && dot(norm, Normal) < 0.0) {
        norm = -norm;
    }

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 ambient = ambientColor;

    vec3 pLightDir = normalize(pointLightPos - FragPos);
    float pDiff = max(dot(norm, pLightDir), 0.0);
    vec3 pDiffuse = pDiff * pointLightColor;
    vec3 pReflect = reflect(-pLightDir, norm);
    float pSpec = pow(max(dot(viewDir, pReflect), 0.0), 32);
    vec3 pSpecular = 0.3 * pSpec * pointLightColor;
    float pDist = length(pointLightPos - FragPos);
    float pAttenuation = 1.0 / (1.0 + 0.02 * pDist + 0.0008 * pDist * pDist);
    vec3 pointLight = (pDiffuse + pSpecular) * pAttenuation;

    vec3 spotLight = vec3(0.0);
    vec3 sLightDir = normalize(spotLightPos - FragPos);
    float theta = dot(sLightDir, normalize(-spotLightDir));
    float epsilon = spotCutOff - spotOuterCutOff;
    float intensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

    if (intensity > 0.0) {
        float sDiff = max(dot(norm, sLightDir), 0.0);
        vec3 sDiffuse = sDiff * spotLightColor;
        vec3 sReflect = reflect(-sLightDir, norm);
        float sSpec = pow(max(dot(viewDir, sReflect), 0.0), 32);
        vec3 sSpecular = 0.5 * sSpec * spotLightColor;
        float sDist = length(spotLightPos - FragPos);
        float sAttenuation = 1.0 / (1.0 + 0.08 * sDist + 0.025 * sDist * sDist);
        spotLight = (sDiffuse + sSpecular) * intensity * sAttenuation;
    }

    vec3 totalLighting = ambient + pointLight + spotLight;
    FragColor = vec4(totalLighting * color, 1.0);
}
