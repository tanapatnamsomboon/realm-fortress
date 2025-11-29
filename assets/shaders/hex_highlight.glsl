#type vertex
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

out vec3 vNormal;
out vec2 vTexCoords;
out vec3 vFragPos;

void main()
{
    vFragPos = vec3(uTransform * vec4(aPosition, 1.0));
    vNormal = mat3(transpose(inverse(uTransform))) * aNormal;
    vTexCoords = aTexCoords;
    gl_Position = uViewProjection * vec4(vFragPos, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 vNormal;
in vec2 vTexCoords;
in vec3 vFragPos;

uniform vec3 uHighlightColor;
uniform float uPulseIntensity;

void main()
{
    vec3 viewDir = normalize(vec3(0.0, 10.0, 10.0) - vFragPos);
    vec3 norm = normalize(vNormal);
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = pow(rim, 2.0);

    vec3 finalColor = uHighlightColor * rim * uPulseIntensity;
    color = vec4(finalColor, 0.5);
}
