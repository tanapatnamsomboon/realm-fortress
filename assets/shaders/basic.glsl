#type vertex
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec3 vFragPos;
out vec3 vNormal;
out vec2 vTexCoords;

void main()
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoords = aTexCoords;

    gl_Position = uViewProjection * worldPos;
}

#type fragment
#version 460 core

layout(location = 0) out vec4 FragColor;

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoords;

uniform sampler2D uTexture;
uniform vec3 uLightDir = vec3(-0.3, -1.0, -0.5);
uniform vec3 uLightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 uAmbientColor = vec3(0.4, 0.4, 0.45);
uniform float uAmbientStrength = 0.3;

void main()
{
    vec4 texColor = texture(uTexture, vTexCoords);

    vec3 ambient = uAmbientStrength * uAmbientColor;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(-uLightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 lighting = ambient + diffuse;
    vec3 result = lighting * texColor.rgb;

    FragColor = vec4(result, texColor.a);
}
