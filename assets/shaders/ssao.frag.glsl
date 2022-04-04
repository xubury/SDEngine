#version 450 core

#include camera.glsl

layout(location = 0) out float frag_color;

layout(location = 0) in vec2 in_uv;

uniform sampler2D u_position;
uniform sampler2D u_normal;

const uint KERNEL_SIZE = 64;
uniform uint u_kernel_size = KERNEL_SIZE;

uniform sampler2D u_noise;
uniform vec3 u_samples[KERNEL_SIZE];
uniform float u_radius;
uniform float u_bias;
uniform uint u_power;

float ComputeOcclusion( vec3 random_vec)
{
    // get input for SSAO algorithm
    vec3 frag_pos = texture(u_position, in_uv).xyz;
    vec3 normal = texture(u_normal, in_uv).xyz;
    if (normal == vec3(0)) return 1;

    frag_pos = (u_view * vec4(frag_pos, 1.0f)).xyz;
    mat3 rot_view = transpose(inverse(mat3(u_view)));
    normal = rot_view * normal;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bi_tangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bi_tangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < u_kernel_size; ++i) {
        // get sample position
        if (dot(u_samples[i], normal) < 0.15) {
            continue;
        }
        vec3 sample_pos = TBN * u_samples[i];  // from tangent to view-space
        sample_pos = frag_pos + sample_pos * u_radius;

        // project sample position (to sample texture)
        // (to get position on screen/texture)
        vec4 offset = vec4(sample_pos, 1.0);
        offset = u_projection * offset;       // from view to clip-space
        offset /= offset.w;                   // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5;  // transform to range 0.0 - 1.0

        if (offset.x < 0 || offset.y < 0 || offset.x > 1 || offset.y > 1) {
            continue;
        }
        // get sample depth
        float sample_depth =
            (u_view * vec4(texture(u_position, offset.xy).xyz, 1.0f)).z;
        if ((rot_view * texture(u_normal, offset.xy).xyz) == vec3(0)) {
            continue;
        }

        // range check & accumulate
        float factor = u_radius / abs(frag_pos.z - sample_depth);
        float range_check = smoothstep(0.0, 1.0, factor);
        occlusion +=
            (sample_depth >= sample_pos.z + u_bias ? 1.0 : 0.0) * range_check;
    }
    return 1 - occlusion / u_kernel_size;
}

void main()
{
    vec2 tex_size = textureSize(u_position, 0);
    vec3 random_vec = texture(u_noise, in_uv * tex_size / 4.f).xyz;

    float occlusion = ComputeOcclusion(random_vec);
    frag_color = pow(occlusion, u_power);
}
