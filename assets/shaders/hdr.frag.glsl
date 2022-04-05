#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 in_tex_coord;

uniform bool u_bloom;
uniform float u_bloomFactor;

uniform float u_exposure;

uniform float u_gamma;

uniform sampler2D u_lighting;
uniform sampler2D u_upsample_buffer;

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;
    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{
    vec3 result = texture(u_lighting, in_tex_coord).rgb;

    // bloom
    if (u_bloom) {
        result += texture(u_upsample_buffer, in_tex_coord).rgb;
    }

    // hdr
    result = ACESToneMapping(result, u_exposure);

    result = pow(result, vec3(1.0 / u_gamma));

    frag_color = vec4(result, 1.0f);
}
