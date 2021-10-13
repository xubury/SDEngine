#shader vertex
#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;

out vec2 texCoord;

void main() {
    texCoord = a_texCoord;
    gl_Position = vec4(a_pos, 1.0);
}

#shader fragment
#version 450 core
out vec4 fragColor;

in vec2 texCoord;

uniform float u_exposure;
uniform bool u_bloom;
uniform float u_bloomFactor;

layout(binding = 0) uniform sampler2D u_lighting;
layout(binding = 1) uniform sampler2D u_blur;

void main() {
    // bloom
    vec3 result = texture(u_lighting, texCoord).rgb;
    if (u_bloom) {
        result += texture(u_blur, texCoord).rgb * u_bloomFactor;
    }
    // hdr
    if (u_exposure > 0) {
        result = vec3(1.0) - exp(-result * u_exposure);
    }
    fragColor = vec4(result, 1.0f);
}
