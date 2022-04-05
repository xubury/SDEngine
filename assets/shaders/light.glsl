struct LightColor {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 pos;
    float constant;
    float linear;
    float quadratic;
};

vec3 CalcDirLight(vec3 light_front, LightColor light_color, vec3 normal, vec3 view_dir,
                  vec3 ambient, vec4 albedo, float shadow)
{
    vec3 lightDir = normalize(-light_front);

    // ambient
    ambient = light_color.ambient * ambient;

    // diffuse
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light_color.diffuse * diff * albedo.rgb;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(view_dir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = light_color.specular * spec * albedo.a;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, LightColor light_color, vec3 frag_pos,
                    vec3 normal, vec3 view_dir, vec3 ambient, vec4 albedo,
                    float shadow)
{
    vec3 lightDir = normalize(light.pos - frag_pos);

    // ambient
    ambient = light_color.ambient * ambient;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light_color.diffuse * diff * albedo.rgb;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(view_dir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
    vec3 specular = light_color.specular * spec * albedo.a;

    float distance = length(light.pos - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + (1.0f - shadow) * (diffuse + specular);
}
