#version 450 core

#include camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bi_tangent;

struct VertexOutput {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 bi_tangent;
};

layout(location = 0) out VertexOutput out_vertex;

uniform mat4 u_model;

void main()
{
    vec3 fragPos = (u_model * vec4(a_pos, 1.0f)).xyz;
    gl_Position = u_projection * u_view * vec4(fragPos, 1.0f);

    mat3 normal_matrix = transpose(inverse(mat3(u_model)));
    out_vertex.position = fragPos;
    out_vertex.normal = normal_matrix * a_normal;
    out_vertex.tangent = normal_matrix * a_tangent;
    out_vertex.bi_tangent = normal_matrix * a_bi_tangent;

    out_vertex.uv = a_uv;
}
