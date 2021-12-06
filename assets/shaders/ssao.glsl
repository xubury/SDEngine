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

#include shaders/camera.glsl

layout(location = 0) out float frag_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2DMS u_position;
layout(binding = 1) uniform sampler2DMS u_normal;

const uint KERNEL_SIZE = 64;
uniform uint u_kernel_size = KERNEL_SIZE;

uniform sampler2D u_noise;
uniform vec3 u_samples[KERNEL_SIZE];
uniform float u_radius;
uniform float u_bias;
uniform uint u_power;

float compute_occlusion(int level, const vec2 tex_size, const ivec2 uv,
                        vec3 random_vec) {
    // get input for SSAO algorithm
    vec3 frag_pos = texelFetch(u_position, uv, level).xyz;
    vec3 normal = texelFetch(u_normal, uv, level).xyz;
    frag_pos = (u_view * vec4(frag_pos, 1.0f)).xyz;
    normal = transpose(inverse(mat3(u_view))) * normal;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bi_tangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bi_tangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < u_kernel_size; ++i) {
        // get sample position
        if (dot(u_samples[i], normal) < 0.05) {
            continue;
        }
        vec3 sample_pos = TBN * u_samples[i]; // from tangent to view-space
        sample_pos = frag_pos + sample_pos * u_radius;

        // project sample position (to sample texture)
        // (to get position on screen/texture)
        vec4 offset = vec4(sample_pos, 1.0);
        offset = u_projection * offset; // from view to clip-space
        offset /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        if (offset.x < 0 || offset.y < 0 || offset.x > 1 || offset.y > 1) {
            continue;
        }
        // get sample depth
        const ivec2 uv = ivec2(offset.xy * tex_size);
        float sample_depth = (u_view * 
                vec4(texelFetch(u_position, uv, level).xyz, 1.0f)).z;

        // range check & accumulate
        float factor = u_radius / abs(frag_pos.z - sample_depth);
        float range_check = smoothstep(0.0, 1.0, factor);
        occlusion += (sample_depth >= sample_pos.z + u_bias ? 1.0 : 0.0) * range_check;
    }
    return 1 - occlusion / u_kernel_size;
}

void main() {
    const vec2 tex_size = textureSize(u_position);
    const vec2 noise_scale = tex_size / 4.f;
    const ivec2 uv = ivec2(in_uv * tex_size);

    vec3 random_vec = texture(u_noise, uv / 4.f).xyz;

    // TODO: MSAA seems not working with SSAO
    // const int num_msaa = textureSamples(u_position);
    const int num_msaa = 1;
    float occlusion = 0;
    for (int i = 0; i < num_msaa; ++i) {
        occlusion += compute_occlusion(i, tex_size, uv, random_vec);
    }
    occlusion /= num_msaa;
    frag_color = pow(occlusion, u_power);
}
