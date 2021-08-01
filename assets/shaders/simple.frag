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
