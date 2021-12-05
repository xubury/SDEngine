#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in float a_tex_id;

struct VertexOutput {
    vec4 color;
    vec2 uv;
    float tex_id;
};

layout(location = 0) out VertexOutput out_vertex;

void main() {
    gl_Position = u_projection * u_view * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_vertex.uv = a_uv;
    out_vertex.tex_id = a_tex_id;
}

#shader fragment
#version 450 core
out vec4 frag_color;

layout(binding = 0) uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 uv;
    float tex_id;
};

layout(location = 0) in VertexOutput in_vertex;

void main() {
    uint id = uint(in_vertex.tex_id + 0.5);
    vec4 textureColor = texture(u_textures[id], in_vertex.uv);
    frag_color = textureColor * in_vertex.color;
}
