#version 450 core
layout (location = 0) out vec4 fragColor;

uniform sampler2D u_textures[32];

struct VertexOutput {
    vec4 color;
    vec2 texCoord;
    float texIndex;
};

layout (location = 0) in VertexOutput vertexInput;

void main() {
    uint texIndex = uint(vertexInput.texIndex);
    fragColor = vertexInput.color * texture(u_textures[texIndex], 
                                            vertexInput.texCoord);
}
