#version 450 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_texIndex;

layout (std140) uniform Camera
{
    mat4 u_projectionView;
};

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) out VertexOutput vertexOutput;

void main() {
    gl_Position = u_projectionView * vec4(a_pos, 1.0f);
    vertexOutput.color = a_color;
    vertexOutput.texCoord = a_texCoord;
    vertexOutput.texIndex = a_texIndex;
}
