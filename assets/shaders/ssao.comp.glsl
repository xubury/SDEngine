#version 450 core
layout(local_size_x = 25, local_size_y = 25) in;
layout(r16f) uniform image2D u_out_image;

#include camera.glsl

uniform sampler2D u_position;
uniform sampler2D u_normal;

const uint KERNEL_SIZE = 64;
uniform uint u_kernel_size = KERNEL_SIZE;

uniform sampler2D u_noise;
uniform vec3 u_samples[KERNEL_SIZE];
uniform float u_radius;
uniform float u_bias;
uniform uint u_power;

float ComputeOcclusion(vec3 random_vec, vec2 uv)
{
    // get input for SSAO algorithm
    vec3 frag_pos = texture(u_position, uv).xyz;
    vec3 normal = texture(u_normal, uv).xyz;
    if (normal == vec3(0)) return 1;

    frag_pos = (u_view * vec4(frag_pos, 1.0f)).xyz;
    mat3 normal_matrix = transpose(inverse(mat3(u_view)));
    normal = normalize(normal_matrix * normal);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bi_tangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bi_tangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < u_kernel_size; ++i) {
        // get sample position
        vec3 sample_pos = TBN * u_samples[i];  // from tangent to view-space
        if(dot(sample_pos, normal) < 0.15) {
            continue;
        }
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
        if ((normal_matrix * texture(u_normal, offset.xy).xyz) == vec3(0)) {
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
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_out_image);
    if (pos.x >= size.x || pos.y >= size.y) {
        return;
    }
    vec2 uv = vec2(pos) / size;
    vec2 random_scale = vec2(size) / 4.f;
    vec3 random_vec = normalize(texture(u_noise, uv * random_scale).xyz);
    float occlusion = ComputeOcclusion(random_vec, uv);
    imageStore(u_out_image, pos, vec4(pow(occlusion, u_power)));
}
