#shader vertex
#version 450 core

#include shaders/camera.glsl

layout (location = 0) in vec3 a_pos;

out vec3 out_uv;

uniform mat4 u_model;

void main() {
    out_uv = a_pos;
    vec4 pos = u_projection_view * u_model * vec4(a_pos, 1.0f);
    gl_Position = pos.xyww;
}

#shader fragment
#version 450 core

out vec4 fragColor;

in vec3 out_uv;

void main() {
    fragColor = vec4(out_uv.y, out_uv.y, out_uv.y , 1.0);
}
