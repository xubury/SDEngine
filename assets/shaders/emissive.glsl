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

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2DMS u_lighting;
layout(binding = 1) uniform sampler2DMS u_emissive;

void main() {
    vec4 color = vec4(0); 

    const int samples = textureSamples(u_lighting);
    const ivec2 uv = ivec2(in_uv * textureSize(u_lighting));
    for (int i = 0; i < samples; ++i) {
        color.rgb += texelFetch(u_lighting, uv, i).rgb;
        color += texelFetch(u_emissive, uv, i);
    }
    frag_color = color / samples;
}
