#shader vertex
#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

layout(location = 0) out vec2 out_uv;

void main() {
    out_uv = a_uv;
    gl_Position = vec4(a_pos, 1.0);
}

#shader fragment
#version 450 core

#include shaders/camera.glsl
#include shaders/light.glsl

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 in_uv;

uniform Light u_light;

layout(binding = 0) uniform sampler2DMS u_lighting;
layout(binding = 1) uniform sampler2DMS u_position;
layout(binding = 2) uniform sampler2DMS u_normal;
layout(binding = 3) uniform sampler2DMS u_albedo;
layout(binding = 4) uniform sampler2DMS u_ambient;
layout(binding = 5) uniform sampler2D u_ssao;


void main() {
    vec3 color = vec3(0);
    const int samples = textureSamples(u_position);
    const float ambient_occlusion = texture(u_ssao, in_uv).r;

    const ivec2 uv = ivec2(in_uv * textureSize(u_position));
    for (int i = 0; i < samples; ++i) {
        vec3 pos = texelFetch(u_position, uv, i).rgb;
        vec3 normal = texelFetch(u_normal, uv, i).rgb;
        vec4 albedo = texelFetch(u_albedo, uv, i);
        vec3 ambient = texelFetch(u_ambient, uv, i).rgb * ambient_occlusion;
        vec3 last = texelFetch(u_lighting, uv, i).rgb;

        vec3 view_pos = u_view[3].xyz;
        vec3 view_dir = normalize(view_pos - pos);
        color += last +
            calculateLight(u_light, pos, normal, view_dir, ambient, albedo);
    }
    fragColor = vec4(color / samples, 1.0f);
}
