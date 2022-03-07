struct Light {
    vec3 position;
    vec3 direction;

    float cutoff;
    float outer_cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    bool is_directional;
    bool is_cast_shadow;

    sampler2DArray cascade_map;
};

layout (std140) uniform LightData
{
    mat4 u_light_matrix[16];
};

uniform float u_cascade_planes[16];
uniform int u_num_of_cascades;
uniform float u_far_plane;

float shadowCalculation(Light light, vec3 frag_pos, vec3 normal, mat4 view) {
    if (!light.is_cast_shadow) return 0.f;

    vec4 frag_pos_view = view * vec4(frag_pos, 1.0f);
    float depth = abs(frag_pos_view.z);

    int layer = u_num_of_cascades;
    for (int i = 0; i < u_num_of_cascades; ++i) {
        if (depth < u_cascade_planes[i]) {
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
    float currentDepth = projCoords.z;

    if (currentDepth > 1.0f) return 0.0f;

    // check whether current frag pos is in shadow
    vec3 lightDir = normalize(light.position - frag_pos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float bias_mod = 0.5f;
    if (layer == u_num_of_cascades) {
        bias *= 1 / (u_far_plane * bias_mod);
    } else {
        bias *= 1 / (u_cascade_planes[layer] * bias_mod);
    }
    bias = 0.05;
    float shadow = 0.0f;

    vec2 tex_size = 1.0f / vec2(textureSize(light.cascade_map, 0));
    const int half_kernel_width = 1;
    for (int x = -half_kernel_width; x <= half_kernel_width; ++x) {
        for (int y = -half_kernel_width; y <= half_kernel_width; ++y) {
            vec3 sample_pos = vec3(projCoords.xy + vec2(x, y) * tex_size, layer);
            float pcfDepth = texture(light.cascade_map, vec3(0)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= (half_kernel_width * 2 + 1) * (half_kernel_width * 2 + 1);

    return shadow;
}

vec3 dirLight(Light light, vec3 frag_pos, vec3 normal, vec3 viewDir,
              vec3 ambient, vec4 albedo, float shadow) {
    vec3 lightDir = normalize(-light.direction);

    // ambient
    ambient = light.ambient * ambient;

    // diffuse
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light.diffuse * diff * albedo.rgb;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = light.specular * spec * albedo.a;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 pointLight(Light light, vec3 frag_pos, vec3 normal, vec3 viewDir,
                vec3 ambient, vec4 albedo, float shadow) {
    vec3 lightDir = normalize(light.position - frag_pos);

    // ambient
    ambient = light.ambient * ambient;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * albedo.rgb;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
    vec3 specular = light.specular * spec * albedo.a;

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // spotlight
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 calculateLight(Light light, vec3 frag_pos, vec3 normal, mat4 view,
                     vec3 ambient, vec4 albedo) {
    vec3 viewDir = normalize(view[3].xyz - frag_pos);
    float shadow = shadowCalculation(light, frag_pos, normal, view);
    return light.is_directional ? dirLight(light, frag_pos, normal, viewDir,
                                            ambient, albedo, shadow)
                                  : pointLight(light, frag_pos, normal, viewDir,
                                            ambient, albedo, shadow);
}
