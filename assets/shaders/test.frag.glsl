#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 in_tex_coord;

uniform sampler2D u_screen;

void main()
{
    frag_color = vec4(texture(u_screen, in_tex_coord).rgb, 1.0);
}
