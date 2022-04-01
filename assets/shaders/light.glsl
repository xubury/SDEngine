struct Light {
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
};

vec3 DirLight(Light light, vec3 light_front, vec3 normal, vec3 viewDir,
              vec3 ambient, vec4 albedo, float shadow)
{
    vec3 lightDir = normalize(-light_front);

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

vec3 PointLight(Light light, vec3 light_pos, vec3 light_front, vec3 frag_pos,
                vec3 normal, vec3 viewDir, vec3 ambient, vec4 albedo,
                float shadow)
{
    vec3 lightDir = normalize(light_pos - frag_pos);

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

    float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // spotlight
    float theta = dot(lightDir, normalize(-light_front));
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 CalculateLight(Light light, vec3 light_pos, vec3 light_front,
                    vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 ambient,
                    vec4 albedo, float shadow)
{
    return light.is_directional
               ? DirLight(light, light_front, normal, viewDir, ambient, albedo,
                          shadow)
               : PointLight(light, light_pos, light_front, frag_pos, normal,
                            viewDir, ambient, albedo, shadow);
}
