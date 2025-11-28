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

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform vec3 uTintColor;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, vTexCoords);

    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    // diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 result = (ambient + diffuse + specular) * texColor.rgb * uTintColor;
    color = vec4(result, 1.0);
}
