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

layout(location = 0) out vec3 frag_color;

layout(location = 0) in vec2 in_uv;

uniform Light u_light;

uniform sampler2DMS u_lighting;
uniform sampler2DMS u_position;
uniform sampler2DMS u_normal;
uniform sampler2DMS u_albedo;
uniform sampler2DMS u_ambient;
uniform sampler2DMS u_background;
uniform sampler2D u_ssao;
uniform bool u_ssao_state;


void main() {
    vec3 color = vec3(0);
    const int samples = textureSamples(u_position);
    const float ambient_occlusion = u_ssao_state ? texture(u_ssao, in_uv).r : 1;

    const ivec2 uv = ivec2(in_uv * textureSize(u_position));
    for (int i = 0; i < samples; ++i) {
        vec3 normal = texelFetch(u_normal, uv, i).rgb;
        if (normal != vec3(0)) {
            vec3 pos = texelFetch(u_position, uv, i).rgb;
            vec3 last = texelFetch(u_lighting, uv, i).rgb;
            // Fragment is not a background,
            // calculate the lighting result
            vec4 albedo = texelFetch(u_albedo, uv, i);
            vec3 ambient = texelFetch(u_ambient, uv, i).rgb * ambient_occlusion;

            vec3 view_pos = u_view[3].xyz;
            vec3 view_dir = normalize(view_pos - pos);
            color += 
                last + calculateLight(u_light, pos, normal, view_dir, ambient, albedo);
        } else {
            // Fragment is background, don't calculate light,
            // just add the last lighting result or the background color.
            color += texelFetch(u_background, uv, i).rgb;
        }
    }
    frag_color = color / samples;
}
