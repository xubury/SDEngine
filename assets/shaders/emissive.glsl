#shader vertex
#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 out_texCoord;

void main() {
    out_texCoord = a_texCoord;
    gl_Position = vec4(a_pos, 1.0);
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 in_texCoord;

layout(binding = 0) uniform sampler2D u_lighting;
layout(binding = 1) uniform sampler2D u_gEmissive;

void main() {
    fragColor = texture(u_lighting, in_texCoord) 
                + vec4(texture(u_gEmissive, in_texCoord).rgb, 1.0f);
}
