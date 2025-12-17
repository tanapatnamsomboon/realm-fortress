#type vertex
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;
    gl_Position = uViewProjection * uModel * vec4(aPosition, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform int uHasTexture;

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 1.5, 1.0));
    vec3 normal = normalize(vNormal);

    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * vec3(1.0);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.8);

    vec4 objectColor = vec4(1.0, 1.0, 1.0, 1.0);

    if (uHasTexture == 1)
    {
        objectColor = texture(uTexture, vTexCoord);

        if (objectColor.a < 0.1)
        discard;
    }

    vec3 result = (ambient + diffuse) * objectColor.rgb;

    color = vec4(result, 1.0);
}
