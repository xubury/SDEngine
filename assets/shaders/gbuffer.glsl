#shader vertex
#version 450 core

#include shaders/camera.glsl

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_biTangent;

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

layout(location = 0) out VertexOutput out_vertex;

uniform mat4 u_model;

void main() {
    vec3 fragPos = (u_model * vec4(a_pos, 1.0f)).xyz;
    gl_Position = u_projectionView * vec4(fragPos, 1.0f);

    out_vertex.position = fragPos;
    out_vertex.normal = transpose(inverse(mat3(u_model))) * a_normal;
    out_vertex.texCoord = a_texCoord;
}

#shader fragment
#version 450 core

#include shaders/material.glsl

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

uniform Material u_material;

uniform vec3 u_color;
uniform uint u_entityId;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec3 gAmbient;
layout(location = 4) out vec3 gEmissive;
layout(location = 5) out uint gEntityId;

layout(location = 0) in VertexOutput in_vertex;

void main() {
    gPosition = in_vertex.position;
    gNormal = normalize(in_vertex.normal);
    vec3 halfColor = u_color * 0.5f;
    gAlbedo.rgb =
        texture(u_material.diffuse, in_vertex.texCoord).rgb + halfColor;
    gAlbedo.a = texture(u_material.specular, in_vertex.texCoord).r;
    gAmbient = texture(u_material.ambient, in_vertex.texCoord).rgb + halfColor;
    gEmissive = texture(u_material.emissive, in_vertex.texCoord).rgb;

    gEntityId = u_entityId;
}
