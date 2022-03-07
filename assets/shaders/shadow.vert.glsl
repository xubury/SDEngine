#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_biTangent;


uniform mat4 u_model;
uniform mat4 u_projection_view;

void main() { gl_Position = u_projection_view * u_model * vec4(a_pos, 1.0f); }
