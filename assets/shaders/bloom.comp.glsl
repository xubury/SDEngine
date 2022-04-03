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

vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve)
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

// FIXME: some point are sample repeatedly
vec4 DownsampleBox13(ImageType src, ivec2 uv)
{
    return  DownsampleBox4(src, uv) * 0.5
            + DownsampleBox4(src, uv + ivec2(1, 1)) * 0.125
            + DownsampleBox4(src, uv + ivec2(-1, 1)) * 0.125
            + DownsampleBox4(src, uv + ivec2(1, -1)) * 0.125
            + DownsampleBox4(src, uv + ivec2(-1, -1)) * 0.125;
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

vec4 CombineUnsample(ImageType src, ImageType blur, ivec2 uv)
{
    return imageLoad(blur, uv) + UpsampleTent(src, uv);
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_out_image);
    if (pos.x >= size.x || pos.y >= size.y) {
        return;
    }
    // check if it is the first input, which is not upsampled or downsampled
    if (u_input) {
        // In downsampling process, first input is the thresholded lighting
        vec4 val = imageLoad(u_in_image, pos);
        if (u_downsample) {
            imageStore(u_out_image, pos, QuadraticThreshold(val, u_threshold, u_curve));
        }
        else {
            imageStore(u_out_image, pos, val);
        }
    }
    else {
        if (u_downsample) {
            imageStore(u_out_image, pos, DownsampleBox13(u_in_image, pos * 2));
        }
        else {
            imageStore(u_out_image, pos, CombineUnsample(u_in_image, u_blur_image, pos / 2));
        }
    }
}
