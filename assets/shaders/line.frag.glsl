#version 450 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out uint entity_id;

struct VertexOutput {
    vec4 color;
};

layout(location = 0) in VertexOutput in_vertex;
layout(location = 1) in flat uint in_entity_id;

void main()
{
    frag_color = in_vertex.color;
    entity_id = in_entity_id;
}
