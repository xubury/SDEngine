struct Light {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    bool isDirectional;
};


vec3 dirLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir,
              vec3 ambient, vec3 diffuse, vec3 specular) {
    vec3 lightDir = normalize(-light.direction);

    // ambient
    ambient = light.ambient * ambient;

    // diffuse
    float diff = max(dot(lightDir, normal), 0.0f);
    diffuse = light.diffuse * diff * diffuse;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    specular = light.specular * spec * specular;

    return ambient + diffuse + specular;
}

vec3 pointLight(Light light, vec3 fragPos, vec3 normal,
                vec3 viewDir, vec3 ambient, vec3 diffuse, vec3 specular) {
    vec3 lightDir = normalize(light.position - fragPos);

    // ambient
    ambient = light.ambient * ambient;

    // diffuse
    float diff = max(dot(lightDir, normal), 0.0f);
    diffuse = light.diffuse * diff * diffuse;

    // specular
    // blinn-phong
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
    specular = light.specular * spec * specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance
                              + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //spotlight
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;

    return ambient + diffuse + specular;
}

vec3 calculateLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir,
                    vec3 ambient, vec3 diffuse, vec3 specular) {
    return light.isDirectional ? dirLight(light, fragPos, normal, viewDir, ambient, diffuse, specular)
                               : pointLight(light, fragPos, normal, viewDir, ambient, diffuse, specular);
}
