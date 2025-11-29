#type vertex
#version 460 core

layout(location = 0) in vec3 vaPosition;
layout(location = 1) in vec3 vaNormal;
layout(location = 2) in vec2 vaTexCoords;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

out vec3 vNormal;
out vec2 vTexCoords;
out vec3 vFragPos;

void main()
{
    vFragPos = vec3(uTransform * vec4(vaPosition, 1.0));
    vNormal = mat3(transpose(inverse(uTransform))) * vaNormal;
    vTexCoords = vaTexCoords;
    gl_Position = uViewProjection * vec4(vFragPos, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 vNormal;
in vec2 vTexCoords;
in vec3 vFragPos;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform vec3 uObjectColor;
uniform float uAlpha;
uniform bool uIsValid;

void main()
{
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * uLightColor;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // determine color based on placement validity
    vec3 baseColor = uIsValid ? uObjectColor : vec3(1.0, 0.2, 0.2);

    vec3 result = (ambient + diffuse) * baseColor;
    color = vec4(result, uAlpha);
}
