#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_texIndex;

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) out VertexOutput out_vertex;


void main() {
    gl_Position = u_projectionView * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_vertex.texCoord = a_texCoord;
    out_vertex.texIndex = a_texIndex;
}

#shader fragment
#version 450 core
out vec4 fragColor;

layout (binding = 0) uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) in VertexOutput in_vertex;

void main() {
    uint id = uint(in_vertex.texIndex);
    vec4 textureColor = texture(u_textures[id], in_vertex.texCoord);
    fragColor = textureColor * in_vertex.color;
}
