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

#include shaders/camera.glsl
#include shaders/light.glsl
#include shaders/material.glsl

struct VertexOutput {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
};

out vec4 fragColor;

layout(location = 0) in VertexOutput in_vertex;

uniform Material u_material;

uniform vec3 u_color;
uniform Light u_light;


void main() {
    vec3 normal = normalize(in_vertex.normal);
    vec3 halfColor = u_color * 0.5f;
    vec3 diffuse = texture(u_material.diffuse, in_vertex.texCoord).rgb + halfColor;
    vec3 specular = vec3(texture(u_material.specular, in_vertex.texCoord).r);
    vec3 ambient = texture(u_material.ambient, in_vertex.texCoord).rgb + halfColor;

    vec3 viewDir = normalize(u_viewPos - in_vertex.position);

    vec3 light = calculateLight(u_light, in_vertex.position, normal, viewDir, ambient,
                                diffuse, specular);
    fragColor = vec4(light, 1.0f);
}
