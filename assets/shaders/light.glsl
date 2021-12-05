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

    sampler2D shadow_map;
    mat4 projection_view;
};

float shadowCalculation(Light light, vec3 fragPos, vec3 normal) {
    if (!light.is_cast_shadow) return 0.f;

    //FIXME: try not to inverse in shader!
    vec4 fragPosLightSpace = light.projection_view * inverse(u_view) 
                             * vec4(fragPos, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    if (projCoords.z > 1.0f) return 0.0f;

    // get closest depth value from light's perspective
    // (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(light.shadow_map, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 light_pos = (u_view * vec4(light.position, 1.0f)).xyz;
    vec3 lightDir = normalize(light_pos - fragPos);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0);
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(light.shadow_map, 0);
    const int halfKernelWidth = 1;
    for (int x = -halfKernelWidth; x <= halfKernelWidth; ++x) {
        for (int y = -halfKernelWidth; y <= halfKernelWidth; ++y) {
            float pcfDepth =
                texture(light.shadow_map, projCoords.xy + vec2(x, y) * texelSize)
                    .r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }
    shadow /= (halfKernelWidth * 2 + 1) * (halfKernelWidth * 2 + 1);

    return shadow;
}

vec3 dirLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir,
              vec3 ambient, vec4 albedo) {
    vec3 lightDir = transpose(inverse(mat3(u_view))) * normalize(-light.direction);

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

    float shadow = shadowCalculation(light, fragPos, normal);
    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 pointLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir,
                vec3 ambient, vec4 albedo) {
    vec3 light_pos = (u_view * vec4(light.position, 1.0f)).xyz;
    vec3 lightDir = normalize(light_pos - fragPos);

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

    float distance = length(light.position - fragPos);
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

    float shadow = shadowCalculation(light, fragPos, normal);
    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 calculateLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir,
                    vec3 ambient, vec4 albedo) {
    return light.is_directional ? dirLight(light, fragPos, normal, viewDir,
                                          ambient, albedo)
                                  : pointLight(light, fragPos, normal, viewDir,
                                            ambient, albedo);
}
