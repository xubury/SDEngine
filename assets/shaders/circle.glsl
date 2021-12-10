#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_world_pos;
layout(location = 1) in vec3 a_local_pos;

struct VertexOutput {
    vec3 local_pos;
};

layout(location = 0) out VertexOutput out_vertex;

void main() {
    gl_Position = u_projection * u_view * vec4(a_world_pos, 1.0f);
    out_vertex.local_pos = a_local_pos;
}


#shader fragment
#version 450 core

out vec4 frag_color;

struct VertexOutput {
    vec3 local_pos;
};

layout(location = 0) in VertexOutput in_vertex;

void main() {
    float fade = 0.05f;
    float thickness = 0.5f;
    float distance = 1.0 - length(in_vertex.local_pos);
    float circle = smoothstep(0.0, fade, distance);
    circle *= smoothstep(thickness + fade, thickness, distance);
    if (circle == 0.0) discard;

    frag_color = vec4(1.0, 1.0f, 1.0f, circle);
}
