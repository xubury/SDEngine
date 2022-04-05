#version 450 core
layout(local_size_x = 13, local_size_y = 13) in;

layout(rgba16f) uniform image2D u_out_image;

uniform sampler2D u_in_texture;
uniform sampler2D u_down_texture;
uniform int u_level;

uniform bool u_input = true;
uniform bool u_downsample = true;
uniform float u_threshold;
uniform vec3 u_curve;

vec3 QuadraticThreshold(vec3 color, float threshold, vec3 curve)
{
    // brightness
    float br = max(color.r, max(color.g, color.b));

    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve
    color *= max(rq, br - threshold) / max(br, 1e-4);
    return color;
}

// Better, temporally stable box filtering
// [Jimenez14] http://goo.gl/eomGso
// . . . . . . .
// . a . b . c .
// . . d . e . .
// . f . g . h .
// . . i . j . .
// . k . l . m .
// . . . . . . .
vec4 DownsampleBox13(sampler2D src, vec2 uv, int level)
{
    vec2 texel_size = 1 / vec2(textureSize(src, level));
    vec4 a = textureLod(src, uv + texel_size * vec2(-2, 2), level);
    vec4 b = textureLod(src, uv + texel_size * vec2(0, 2), level);
    vec4 c = textureLod(src, uv + texel_size * vec2(2, 2), level);
    vec4 d = textureLod(src, uv + texel_size * vec2(1, 1), level);
    vec4 e = textureLod(src, uv + texel_size * vec2(1, -1), level);
    vec4 f = textureLod(src, uv + texel_size * vec2(-2, 0), level);
    vec4 g = textureLod(src, uv, level);
    vec4 h = textureLod(src, uv + texel_size * vec2(2, 0), level);
    vec4 i = textureLod(src, uv + texel_size * vec2(-1, -1), level);
    vec4 j = textureLod(src, uv + texel_size * vec2(1, -1), level);
    vec4 k = textureLod(src, uv + texel_size * vec2(-2, -2), level);
    vec4 l = textureLod(src, uv + texel_size * vec2(0, -2), level);
    vec4 m = textureLod(src, uv + texel_size * vec2(2, -2), level);

    vec2 div = 0.25 * vec2(0.5, 0.125);
    vec4 val = (d + e + i + j) * div.x;
    val += (a + b + g + f) * div.y;
    val += (b + c + h + g) * div.y;
    val += (f + g + l + k) * div.y;
    val += (g + h + l + m) * div.y;
    return val;
}

vec4 UpsampleTent(sampler2D src, vec2 uv, int level)
{
    vec2 texel_size = 1 / vec2(textureSize(src, level));
    vec4 value = textureLod(src, uv, level) * 4
                + textureLod(src, uv + texel_size * vec2(1, 0), level) * 2
                + textureLod(src, uv + texel_size * vec2(-1, 0), level) * 2
                + textureLod(src, uv + texel_size * vec2(0, 1), level) * 2
                + textureLod(src, uv + texel_size * vec2(0, -1), level) * 2
                + textureLod(src, uv + texel_size * vec2(1, 1), level)
                + textureLod(src, uv + texel_size * vec2(-1, 1), level)
                + textureLod(src, uv + texel_size * vec2(1, -1), level)
                + textureLod(src, uv + texel_size * vec2(-1, -1), level);
    return value / 16.f;
}

vec4 Downsample(vec2 uv)
{
    // check if it is the first input
    if (u_input) {
        vec4 val = textureLod(u_in_texture, uv, 0);
        val.rgb = QuadraticThreshold(val.rgb, u_threshold, u_curve);
        return val;
    }
    else {
        return DownsampleBox13(u_in_texture, uv, u_level);
    }
}

vec4 Upsample(vec2 uv)
{
    // check if it is the first input
    if (u_input) {
        return UpsampleTent(u_in_texture, uv, u_level);
    }
    else {
        return textureLod(u_down_texture, uv, u_level - 1) 
               + UpsampleTent(u_in_texture, uv, u_level);
    }
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_out_image);
    if (pos.x >= size.x || pos.y >= size.y) {
        return;
    }
    // There will be pixel shift without half pixel offset
    vec2 uv = (pos + vec2(0.5, 0.5)) / size;
    imageStore(u_out_image, pos, u_downsample ? Downsample(uv) : Upsample(uv));
}
