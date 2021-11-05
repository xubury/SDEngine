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

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 in_texCoord;

uniform Light u_light;

layout(binding = 0) uniform sampler2D u_lighting;
layout(binding = 1) uniform sampler2DMS u_gPosition;
layout(binding = 2) uniform sampler2DMS u_gNormal;
layout(binding = 3) uniform sampler2DMS u_gAlbedo;
layout(binding = 4) uniform sampler2DMS u_gAmbient;


void main() {
    vec3 cur = vec3(0);
    const ivec2 uv = ivec2(in_texCoord * textureSize(u_gPosition));
    const int samples = textureSamples(u_gPosition);
    for (int i = 0; i < samples; ++i) {
        vec3 fragPos = texelFetch(u_gPosition, uv, i).rgb;
        vec3 normal = texelFetch(u_gNormal, uv, i).rgb;
        vec4 albedo = texelFetch(u_gAlbedo, uv, i);
        vec3 diffuse = albedo.rgb;
        vec3 specular = vec3(albedo.a);
        vec3 ambient = texelFetch(u_gAmbient, uv, i).rgb;

        vec3 viewDir = normalize(u_viewPos - fragPos);
        cur += calculateLight(u_light, fragPos, normal, viewDir, ambient,
                              diffuse, specular);
    }
    cur = cur / samples;

    vec3 last = texture(u_lighting, in_texCoord).rgb;
    vec3 result = last + cur;
    fragColor = vec4(result, 1.0f);
}
