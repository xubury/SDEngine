struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D ambient;
    sampler2D emissive;
    sampler2D normal;

    vec3 diffuse_color;
    vec3 ambient_color;
    vec3 emissive_color;
};
