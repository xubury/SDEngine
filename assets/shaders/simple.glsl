#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_texIndex;
layout (location = 4) in uint a_grayScale;
layout (location = 5) in uint a_entityIndex;

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) out VertexOutput out_vertex;
layout (location = 5) out flat uint out_entityIndex;
layout (location = 6) out flat uint out_grayScale;


void main() {
    gl_Position = u_projectionView * vec4(a_pos, 1.0f);
    out_vertex.color = a_color;
    out_vertex.texCoord = a_texCoord;
    out_vertex.texIndex = a_texIndex;
    out_grayScale = a_grayScale;
    out_entityIndex = a_entityIndex;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out uint entityIndex;

layout (binding = 0) uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) in VertexOutput in_vertex;
layout (location = 5) in flat uint in_entityIndex;
layout (location = 6) in flat uint in_grayScale;

void main() {
    uint id = uint(in_vertex.texIndex);
    if (in_grayScale != 0) {
        fragColor = vec4(in_vertex.color.rgb, texture(u_textures[id], in_vertex.texCoord).r);
    } else {
        fragColor = in_vertex.color * texture(u_textures[id], in_vertex.texCoord);
    }
    entityIndex = in_entityIndex;
}
