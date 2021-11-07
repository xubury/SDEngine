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

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 in_texCoord;

layout(binding = 0) uniform sampler2DMS u_lighting;
layout(binding = 1) uniform sampler2DMS u_gEmissive;

void main() {
    vec4 color = vec4(0); 

    const int samples = textureSamples(u_lighting);
    const ivec2 uv = ivec2(in_texCoord * textureSize(u_lighting));
    for (int i = 0; i < samples; ++i) {
        color += texelFetch(u_lighting, uv, i) + texelFetch(u_gEmissive, uv, i);
    }
    fragColor = color / samples;
}
