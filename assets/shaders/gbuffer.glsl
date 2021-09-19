#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_biTangent;

struct VertexOutput {
    vec3 position;
};

layout (location = 0) out VertexOutput out_vertex;

uniform mat4 u_world;

void main() {
    vec3 fragPos = (u_world * vec4(a_pos, 1.0f)).xyz;
    gl_Position = u_projectionView * vec4(fragPos, 1.0f);

    out_vertex.position = fragPos;
}

#shader fragment
#version 450 core

struct VertexOutput {
    vec3 position;
};

layout (location = 0) out vec4 fragPos;

layout (location = 0) in VertexOutput in_vertex;

void main() {
    fragPos = vec4(in_vertex.position, 1.0f);
}
