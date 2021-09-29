#shader vertex
#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;

layout (location = 0) out vec2 out_texCoord;

void main() {
    out_texCoord = a_texCoord;
    gl_Position = vec4(a_pos, 1.0);
}

#shader fragment
#version 450 core

#include shaders/camera.glsl
#include shaders/light.glsl

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 in_texCoord;

uniform Light u_light;

uniform sampler2D u_lighting;
uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbedo;
uniform sampler2D u_gAmbient;

void main() {
    vec3 fragPos = texture(u_gPosition, in_texCoord).rgb;
    vec3 normal = texture(u_gNormal, in_texCoord).rgb;
    normal = normalize(normal);
    vec4 albedo = texture(u_gAlbedo, in_texCoord);
    vec3 diffuse = albedo.rgb;
    vec3 specular = vec3(albedo.a);
    vec3 ambient = texture(u_gAmbient, in_texCoord).rgb;

    vec3 viewDir = normalize(u_viewPos - fragPos);

    vec3 last = texture(u_lighting, in_texCoord).rgb;
    vec3 cur = calculateLight(u_light, fragPos, normal, viewDir, ambient, diffuse, specular);
    vec3 result = last + cur;
    fragColor = vec4(result, 1.0f);
}
