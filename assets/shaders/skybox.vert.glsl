#version 450 core

layout(location = 0) in vec3 a_pos;

out vec3 out_uv;

uniform mat4 u_projection;

void main()
{
    out_uv = a_pos;
    vec4 pos = u_projection * vec4(a_pos, 1.0f);
    gl_Position = pos.xyww;
}
