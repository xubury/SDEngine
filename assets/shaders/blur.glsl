#shader vertex
#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;

layout (location = 0) out vec2 out_texCoord;

void main() {
    out_texCoord = a_texCoord;
    gl_Position = vec4(a_pos, 1.0);
}


#shader fragment
#version 450 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 in_texCoord;

uniform sampler2D u_image;
uniform bool u_horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 tex_offset = 1.0 / textureSize(u_image, 0); // gets size of single texel
    vec3 result = texture(u_image, in_texCoord).rgb * weight[0]; // current fragment's contribution
    if(u_horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_image, in_texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_image, in_texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_image, in_texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_image, in_texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0);
}