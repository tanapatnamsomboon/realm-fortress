#version 460 core

in vec3 vNormal;
in vec2 vUV;

out vec4 FragColor;

uniform sampler2D uTexture0;
uniform vec3 uLightDir = normalize(vec3(-0.3, -1.0, -0.2));
uniform vec3 uTint = vec3(1.0);

void main()
{
    vec3 normal = normalize(vNormal);
    float diff = max(dot(normal, -uLightDir), 0.25);
    vec3 base = texture(uTexture0, vUV).rgb * uTint;
    FragColor = vec4(base * diff, 1.0);
}
