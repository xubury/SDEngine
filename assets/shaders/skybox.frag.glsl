#version 450 core

layout(location = 0) out vec4 frag_color;

in vec3 out_uv;

uniform samplerCube u_skybox;

void main() { frag_color = texture(u_skybox, out_uv); }
