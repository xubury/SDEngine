#shader vertex
#version 450 core

#include camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in int a_tex_id;
layout(location = 4) in uint a_entity_id;

struct VertexOutput {
    vec4 color;
    vec2 uv;
};

layout(location = 0) out VertexOutput out_vertex;
layout(location = 2) out flat int out_tex_id;
layout(location = 3) out flat uint out_entity_id;

void main() {
    gl_Position = u_projection * u_view * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_vertex.uv = a_uv;
    out_tex_id = a_tex_id;
    out_entity_id = a_entity_id;
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out uint entity_id;

layout(binding = 0) uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 uv;
};

layout(location = 0) in VertexOutput in_vertex;
layout(location = 2) in flat int in_tex_id;
layout(location = 3) in flat uint in_entity_id;

void main() {
    vec4 textureColor = texture(u_textures[in_tex_id], in_vertex.uv);
    frag_color = textureColor * in_vertex.color;
    entity_id = in_entity_id;
}
