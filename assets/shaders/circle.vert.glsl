#version 450 core

#include camera.glsl

layout(location = 0) in vec3 a_world_pos;
layout(location = 1) in vec3 a_local_pos;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;
layout(location = 5) in uint a_entity_id;

struct VertexOutput {
    vec3 local_pos;
    vec4 color;
    float thickness;
    float fade;
};

layout(location = 0) out VertexOutput out_vertex;
layout(location = 5) out flat uint out_entity_id;

void main()
{
    gl_Position = u_projection * u_view * vec4(a_world_pos, 1.0f);
    out_vertex.local_pos = a_local_pos;
    out_vertex.color = a_color;
    out_vertex.thickness = a_thickness;
    out_vertex.fade = a_fade;
    out_entity_id = a_entity_id;
}
