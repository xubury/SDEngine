#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_biTangent;

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

layout (location = 0) out VertexOutput out_vertex;

uniform mat4 u_world;

void main() {
    vec3 fragPos = (u_world * vec4(a_pos, 1.0f)).xyz;
    gl_Position = u_projectionView * vec4(fragPos, 1.0f);

    out_vertex.position = fragPos;
    out_vertex.normal = normalize(mat3(transpose(inverse(u_world))) * a_normal);
    out_vertex.texCoord = a_texCoord;
}

#shader fragment
#version 450 core

#include shaders/material.glsl

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

uniform vec3 u_color;
uniform Material u_material;
uniform uint u_entityId;

layout (location = 0) out vec4 g_position;
layout (location = 1) out vec4 g_normal;
layout (location = 2) out vec4 g_albedo;
layout (location = 3) out vec4 g_ambient;
layout (location = 4) out uint entityId;

layout (location = 0) in VertexOutput in_vertex;

void main() {
    g_position = vec4(in_vertex.position, 1.0f);
    g_normal = vec4(in_vertex.normal, 1.0f);
    vec3 halfColor = u_color * 0.5f;
    g_albedo.rgb = texture(u_material.diffuse, in_vertex.texCoord).rgb + halfColor;
    g_albedo.a = texture(u_material.specular, in_vertex.texCoord).r;
    g_ambient = vec4(texture(u_material.ambient, in_vertex.texCoord).rgb + halfColor, 1.f);

    entityId = u_entityId;
}
