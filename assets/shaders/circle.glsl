#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_world_pos;
layout(location = 1) in vec3 a_local_pos;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_thickness;
layout(location = 4) in float a_fade;

struct VertexOutput {
    vec3 local_pos;
    vec4 color;
    float thickness;
    float fade;
};

layout(location = 0) out VertexOutput out_vertex;

void main() {
    gl_Position = u_projection * u_view * vec4(a_world_pos, 1.0f);
    out_vertex.local_pos = a_local_pos;
    out_vertex.color = a_color;
    out_vertex.thickness = a_thickness;
    out_vertex.fade = a_fade;
}


#shader fragment
#version 450 core

out vec4 frag_color;

struct VertexOutput {
    vec3 local_pos;
    vec4 color;
    float thickness;
    float fade;
};

layout(location = 0) in VertexOutput in_vertex;

void main() {
    float distance = 1.0 - length(in_vertex.local_pos);
    float circle = smoothstep(0.0, in_vertex.fade, distance);
    circle *= smoothstep(in_vertex.thickness + in_vertex.fade,
                         in_vertex.thickness, distance);

    if (circle == 0.0) discard;
    frag_color = in_vertex.color;
    frag_color.a *= circle;
}