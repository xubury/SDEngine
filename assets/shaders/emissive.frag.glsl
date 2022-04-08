#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 in_uv;

uniform sampler2DMS u_lighting;
uniform sampler2DMS u_emissive;

void main()
{
    vec3 color = vec3(0);

    const int samples = textureSamples(u_lighting);
    const ivec2 uv = ivec2(in_uv * textureSize(u_lighting));
    for (int i = 0; i < samples; ++i) {
        color += texelFetch(u_lighting, uv, i).rgb;
        color += texelFetch(u_emissive, uv, i).rgb;
    }
    frag_color = vec4(color / samples, 1.0f);
}
