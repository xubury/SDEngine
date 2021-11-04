#shader vertex
#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 out_texCoord;

void main() {
    out_texCoord = a_texCoord;
    gl_Position = vec4(a_pos, 1.0);
}

#shader fragment
#version 450 core

#include shaders/camera.glsl
#include shaders/light.glsl
#include shaders/textureMS.glsl

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 in_texCoord;

uniform Light u_light;

layout(binding = 0) uniform sampler2DMS u_lighting;
layout(binding = 1) uniform sampler2DMS u_gPosition;
layout(binding = 2) uniform sampler2DMS u_gNormal;
layout(binding = 3) uniform sampler2DMS u_gAlbedo;
layout(binding = 4) uniform sampler2DMS u_gAmbient;


void main() {
    vec3 fragPos = textureMS(u_gPosition, in_texCoord).rgb;
    vec3 normal = textureMS(u_gNormal, in_texCoord).rgb;
    vec4 albedo = textureMS(u_gAlbedo, in_texCoord);
    vec3 diffuse = albedo.rgb;
    vec3 specular = vec3(albedo.a);
    vec3 ambient = textureMS(u_gAmbient, in_texCoord).rgb;

    vec3 viewDir = normalize(u_viewPos - fragPos);

    vec3 last = textureMS(u_lighting, in_texCoord).rgb;
    vec3 cur = calculateLight(u_light, fragPos, normal, viewDir, ambient,
                              diffuse, specular);
    vec3 result = last + cur;
    fragColor = vec4(result, 1.0f);
}
