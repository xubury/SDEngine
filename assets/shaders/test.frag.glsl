#version 450 core

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 in_tex_coord;

uniform sampler2D u_screen;
uniform int u_level = 0;

void main()
{
    const vec2 tex_offset = 1 / textureSize(u_screen, u_level);
    const vec2 uv =  in_tex_coord;
    const vec3 color = texture(u_screen, uv).rgb;
    const float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 0.5) {
        frag_color = vec4(color, 1.0);
    } else {
        frag_color = vec4(0, 0, 0, 1);
    }

    frag_color = vec4(color, 1.0);
}
