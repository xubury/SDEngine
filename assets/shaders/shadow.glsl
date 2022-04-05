layout(std140) uniform ShadowData { mat4 u_light_matrix[16]; };

uniform int u_num_of_cascades;
uniform float u_cascade_planes[4];
uniform sampler2DArray u_cascade_map;

const int half_kernel_width = 1;
const float bias_mod = 0.5f;

float DirShadowCalculation(vec3 light_dir, vec3 frag_pos, vec3 normal, mat4 view)
{
    vec4 frag_pos_view = view * vec4(frag_pos, 1.0f);
    float depthValue = abs(frag_pos_view.z);

    int layer = u_num_of_cascades - 1;
    for (int i = 0; i < u_num_of_cascades; ++i) {
        if (depthValue < u_cascade_planes[i]) {
            layer = i;
            break;
        }
    }
    vec4 frag_pos_light = u_light_matrix[layer] * vec4(frag_pos, 1.0);
    // perform perspective divide
    vec3 projCoords = frag_pos_light.xyz / frag_pos_light.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // get depth of current fragment from light's perspective
    float current_depth = projCoords.z;

    if (current_depth > 1.0f) return 0.0f;

    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    bias *= 1 / (u_cascade_planes[layer] * bias_mod);

    float shadow = 0.0f;
    // PCF
    vec2 tex_size = 1.0f / vec2(textureSize(u_cascade_map, 0));
    for (int x = -half_kernel_width; x <= half_kernel_width; ++x) {
        for (int y = -half_kernel_width; y <= half_kernel_width; ++y) {
            vec3 sample_pos =
                vec3(projCoords.xy + vec2(x, y) * tex_size, layer);
            float pcfDepth = texture(u_cascade_map, sample_pos).r;
            shadow += (current_depth - bias) > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= (half_kernel_width * 2 + 1) * (half_kernel_width * 2 + 1);

    return shadow;
}

uniform samplerCube u_point_shadow_map;
uniform float u_point_shadow_far_z;

// array of offset direction for sampling
vec3 sample_disk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float CubeShadowCalculation(vec3 frag_pos, vec3 light_pos, vec3 view_pos)
{
    // get vector between fragment position and light position
    vec3 frag_to_light = frag_pos - light_pos;
    // now get current linear depth as the length between the fragment and light position
    float current_depth = length(frag_to_light);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float view_dist = length(view_pos - frag_pos);
    float disk_r = (1.0 + (view_dist / u_point_shadow_far_z)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closest = texture(u_point_shadow_map,
                                    frag_to_light + sample_disk[i] * disk_r).r;
        closest *= u_point_shadow_far_z;   // undo mapping [0;1]
        if(current_depth - bias > closest) 
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}
