#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;
layout(location = 2) in int a_entity_id;

struct VertexOutput {
    vec4 color;
};

layout(location = 0) out VertexOutput out_vertex;
layout(location = 1) out flat int out_entity_id;

void main() {
    gl_Position = u_projection * u_view * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_entity_id = a_entity_id;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out int entity_id;

struct VertexOutput {
    vec4 color;
};

layout(location = 0) in VertexOutput in_vertex;
layout(location = 1) in flat int in_entity_id;

void main() {
    frag_color = in_vertex.color;
    entity_id = in_entity_id;
}
