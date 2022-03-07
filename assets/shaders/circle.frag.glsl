#version 450 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out uint entity_id;

struct VertexOutput {
    vec3 local_pos;
    vec4 color;
    float thickness;
    float fade;
};

layout(location = 0) in VertexOutput in_vertex;
layout(location = 5) in flat uint in_entity_id;

void main() {
    float distance = 1.0 - length(in_vertex.local_pos);
    float circle = smoothstep(0.0, in_vertex.fade, distance);
    circle *= smoothstep(in_vertex.thickness + in_vertex.fade,
                         in_vertex.thickness, distance);

    if (circle == 0.0) discard;
    frag_color = in_vertex.color;
    frag_color.a *= circle;

    entity_id = in_entity_id;
}
