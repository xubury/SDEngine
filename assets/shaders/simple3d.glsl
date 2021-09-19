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

layout (location = 0) out VertexOutput out_vertex;

uniform mat4 u_world;

void main() {
    gl_Position = u_projectionView * u_world * vec4(a_pos, 1.0f);
    out_vertex.texCoord = a_texCoord;
}

#shader fragment
#version 450 core

#include shaders/material.glsl
#include shaders/light.glsl

layout (location = 0) out vec4 fragColor;
layout (location = 1) out uint entityId;

struct VertexOutput {
    vec2 texCoord;
};

layout (location = 0) in VertexOutput in_vertex;

uniform uint u_entityId;

uniform Material u_material;

void main() {
    vec3 diffuse = texture(u_material.diffuse, in_vertex.texCoord).rgb;
    vec3 ambient = texture(u_material.ambient, in_vertex.texCoord).rgb;
    vec3 specular = texture(u_material.specular, in_vertex.texCoord).rgb;
    fragColor = vec4(ambient + diffuse + specular, 1.0f);
    entityId = u_entityId;
}
