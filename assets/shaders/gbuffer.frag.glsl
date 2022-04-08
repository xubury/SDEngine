#version 450 core

#include material.glsl

struct VertexOutput {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 bi_tangent;
};

uniform Material u_material;
uniform uint u_entity_id;

layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo;
layout(location = 3) out vec3 g_ambient;
layout(location = 4) out vec3 g_emissive;
layout(location = 5) out uint g_entity_id;

layout(location = 0) in VertexOutput in_vertex;

void main()
{
    g_position = in_vertex.position;
    g_normal = normalize(in_vertex.normal);
    const vec3 normal_map = texture(u_material.normal, in_vertex.uv).rgb;
    if (normal_map != vec3(0)) {
        vec3 tangent = normalize(in_vertex.tangent);
        tangent = normalize(tangent - dot(tangent, g_normal) * g_normal);
        vec3 bi_tangent = normalize(in_vertex.bi_tangent);
        mat3 tbn = mat3(tangent, bi_tangent, g_normal);
        const vec3 height = normalize(normal_map * 2.0f - 1.0f);
        g_normal = tbn * height;
    }

    g_albedo.rgb = texture(u_material.diffuse, in_vertex.uv).rgb * u_material.diffuse_color;
    g_albedo.a = texture(u_material.specular, in_vertex.uv).r;
    g_ambient = texture(u_material.ambient, in_vertex.uv).rgb * u_material.ambient_color;
    g_emissive = texture(u_material.emissive, in_vertex.uv).rgb + u_material.emissive_color;

    g_entity_id = u_entity_id;
}
