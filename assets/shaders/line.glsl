#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;

struct VertexOutput {
    vec4 color;
};

layout(location = 0) out VertexOutput out_vertex;

void main() {
    gl_Position = u_projection * u_view * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
}


#shader fragment
#version 450 core

out vec4 frag_color;

struct VertexOutput {
    vec4 color;
};

layout(location = 0) in VertexOutput in_vertex;

void main() {
    frag_color = in_vertex.color;
}
