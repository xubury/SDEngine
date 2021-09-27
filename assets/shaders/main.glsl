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
layout (location = 1) out uint entityId;

layout (location = 0) in vec2 in_texCoord;

uniform float u_exposure;
uniform sampler2D u_lighting;
uniform sampler2D u_blur;
uniform usampler2D u_entityTexture;

void main() {
    // bloom
    vec3 result = texture(u_lighting, in_texCoord).rgb + texture(u_blur, in_texCoord).rgb;
    // hdr
    if (u_exposure > 0) {
        result = vec3(1.0) - exp(-result * u_exposure);
    }
    fragColor = vec4(result, 1.0f);
    entityId = texture(u_entityTexture, in_texCoord).r;
}
