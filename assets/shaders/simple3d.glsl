#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_biTangent;

struct VertexOutput {
    vec2 texCoord;
};

out VertexOutput out_vertex;

void main() {
    gl_Position = u_projectionView * u_world * vec4(a_pos, 1.0f);
    out_vertex.texCoord = a_texCoord;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out uint entityId;

struct VertexOutput {
    vec2 texCoord;
};

in VertexOutput in_vertex;

uniform uint u_entityId;

void main() {
    fragColor = vec4(1, 0, 0, 1);
    entityId = u_entityId;
}
