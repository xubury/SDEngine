#version 450 core

#include material.glsl

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

uniform Material u_material;

uniform vec3 u_color;
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
    vec3 halfColor = u_color * 0.5f;
    g_albedo.rgb =
        texture(u_material.diffuse, in_vertex.texCoord).rgb + halfColor;
    g_albedo.a = texture(u_material.specular, in_vertex.texCoord).r;
    g_ambient = texture(u_material.ambient, in_vertex.texCoord).rgb + halfColor;
    g_emissive = texture(u_material.emissive, in_vertex.texCoord).rgb;

    g_entity_id = u_entity_id;
}
