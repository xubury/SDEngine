#version 450 core
layout(local_size_x = 13, local_size_y = 13) in;

#define ImageType layout(rgba16f) image2D

layout(rgba16f) uniform image2D u_in_image;
layout(rgba16f) uniform image2D u_out_image;
layout(rgba16f) uniform image2D u_blur_image;

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

vec4 DownsampleBox4(ImageType src, ivec2 uv)
{
    return (imageLoad(src, uv + ivec2(1, 1))
            + imageLoad(src, uv + ivec2(-1, 1))
            + imageLoad(src, uv + ivec2(1, -1))
            + imageLoad(src, uv + ivec2(-1, -1))) * 0.25;
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
vec4 DownsampleBox13(ImageType src, ivec2 uv)
{
    vec4 a = imageLoad(src, uv + ivec2(-2, 2));
    vec4 b = imageLoad(src, uv + ivec2(0, 2));
    vec4 c = imageLoad(src, uv + ivec2(2, 2));
    vec4 d = imageLoad(src, uv + ivec2(1, 1));
    vec4 e = imageLoad(src, uv + ivec2(1, -1));
    vec4 f = imageLoad(src, uv + ivec2(-2, 0));
    vec4 g = imageLoad(src, uv);
    vec4 h = imageLoad(src, uv + ivec2(2, 0));
    vec4 i = imageLoad(src, uv + ivec2(-1, -1));
    vec4 j = imageLoad(src, uv + ivec2(1, -1));
    vec4 k = imageLoad(src, uv + ivec2(-2, -2));
    vec4 l = imageLoad(src, uv + ivec2(0, -2));
    vec4 m = imageLoad(src, uv + ivec2(2, -2));

    vec2 div = 0.25 * vec2(0.5, 0.125);
    vec4 val = (d + e + i + j) * div.x;
    val += (a + b + g + f) * div.y;
    val += (b + c + h + g) * div.y;
    val += (f + g + l + k) * div.y;
    val += (g + h + l + m) * div.y;
    return val;
}

vec4 UpsampleTent(ImageType src, ivec2 uv)
{
    vec4 value = imageLoad(src, uv) * 4
                + imageLoad(src, uv + ivec2(1, 0)) * 2
                + imageLoad(src, uv + ivec2(-1, 0)) * 2
                + imageLoad(src, uv + ivec2(0, 1)) * 2
                + imageLoad(src, uv + ivec2(0, -1)) * 2
                + imageLoad(src, uv + ivec2(1, 1))
                + imageLoad(src, uv + ivec2(-1, 1))
                + imageLoad(src, uv + ivec2(1, -1))
                + imageLoad(src, uv + ivec2(-1, -1));
    return value / 16.f;
}

vec4 CombineUnsample(ImageType src, ivec2 uv)
{
    return imageLoad(u_blur_image, uv) + UpsampleTent(src, uv);
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_out_image);
    if (pos.x >= size.x || pos.y >= size.y) {
        return;
    }
    vec2 scale = vec2(imageSize(u_in_image)) / size;
    ivec2 uv = ivec2(vec2(pos) * scale);
    // check if it is the first input
    if (u_input) {
        // In downsampling process, first input is the thresholded lighting
        if (u_downsample) {
            vec4 val = imageLoad(u_in_image, uv);
            val.rgb = QuadraticThreshold(val.rgb, u_threshold, u_curve);
            imageStore(u_out_image, pos, val);
        }
        else {
            imageStore(u_out_image, pos, UpsampleTent(u_in_image, uv));
        }
    }
    else {
        if (u_downsample) {
            imageStore(u_out_image, pos, DownsampleBox13(u_in_image, uv));
        }
        else {
            imageStore(u_out_image, pos, CombineUnsample(u_in_image, uv));
        }
    }
}
