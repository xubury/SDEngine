#shader vertex
#version 450 core

#include camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in uint a_texIndex;

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
};

layout (location = 0) out VertexOutput out_vertex;
layout (location = 2) out flat uint out_texIndex;

void main() {
    gl_Position = u_projectionView * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_vertex.texCoord = a_texCoord;
    out_texIndex = a_texIndex;
}

#shader fragment
#version 450 core
layout (location = 0) out vec4 fragColor;

uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
};

layout (location = 0) in VertexOutput in_vertex;
layout (location = 2) in flat uint in_texIndex;

void main() {
    fragColor = in_vertex.color * texture(u_textures[in_texIndex],
                                          in_vertex.texCoord);
}