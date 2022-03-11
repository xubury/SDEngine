layout (std140) uniform LightData
{
    mat4 u_light_matrix[16];
};

uniform sampler2DArray u_cascade_map;
uniform int u_num_of_cascades;
uniform int u_layer;
uniform float u_cascade_planes[16];

float shadowCalculation(vec3 light_dir, vec3 frag_pos, vec3 normal, mat4 view) {
    vec4 frag_pos_view = view * vec4(frag_pos, 1.0f);
    float depthValue = abs(frag_pos_view.z);

    int layer = u_num_of_cascades -1;
    for (int i = 0; i < u_num_of_cascades; ++i) {
        if (depthValue < u_cascade_planes[i]) {
            layer = i;
            break;
        }
    }
    // FIXME:WTF:
    // with the next line the skybox will fail, 
    // without it shadow doesn't work.
    // layer = u_debug ? layer : u_layer;
    vec4 frag_pos_light = u_light_matrix[layer] * vec4(frag_pos, 1.0);
    // perform perspective divide
    vec3 projCoords = frag_pos_light.xyz / frag_pos_light.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    if (currentDepth > 1.0f) return 0.0f;

    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    const float bias_mod = 0.5f;
    bias *= 1 / (u_cascade_planes[layer] * bias_mod);

    float shadow = 0.0f;

    //PCF
    vec2 tex_size = 1.0f / vec2(textureSize(u_cascade_map, 0));
    const int half_kernel_width = 1;
    for (int x = -half_kernel_width; x <= half_kernel_width; ++x) {
        for (int y = -half_kernel_width; y <= half_kernel_width; ++y) {
            vec3 sample_pos = vec3(projCoords.xy + vec2(x, y) * tex_size, layer);
            float pcfDepth = texture(u_cascade_map, sample_pos).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= (half_kernel_width * 2 + 1) * (half_kernel_width * 2 + 1);

    return shadow;
}

