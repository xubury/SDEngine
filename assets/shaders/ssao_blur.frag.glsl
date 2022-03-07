#version 450

out float frag_color;

layout(location = 0) in vec2 in_uv;

uniform sampler2D u_ssao;

void main()
{
    vec2 tex_sizse = 1.0 / vec2(textureSize(u_ssao, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * tex_sizse;
            result += texture(u_ssao, in_uv + offset).r;
        }
    }
    frag_color = result / (4.0 * 4.0);
}
