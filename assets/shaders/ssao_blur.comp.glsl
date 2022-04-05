#version 450
layout(local_size_x = 25, local_size_y = 25) in;

layout(r16f) uniform image2D u_out_image;

uniform sampler2D u_input;

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_out_image);
    if (pos.x >= size.x || pos.y >= size.y) {
        return;
    }
    vec2 uv = vec2(pos) / size;
    vec2 texel_size = 1.0 / vec2(textureSize(u_input, 0));
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(x, y) * texel_size;
            result += texture(u_input, uv + offset).r;
        }
    }
    imageStore(u_out_image, pos, vec4(result / (5.0 * 5.0)));
}
