#version 450 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out uint entity_id;

uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 uv;
};

layout(location = 0) in VertexOutput in_vertex;
layout(location = 2) in flat int in_tex_id;
layout(location = 3) in flat uint in_entity_id;

void main()
{
    vec4 textureColor = texture(u_textures[in_tex_id], in_vertex.uv);
    frag_color = textureColor * in_vertex.color;
    entity_id = in_entity_id;
}
