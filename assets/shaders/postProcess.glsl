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

uniform bool u_bloom;
uniform float u_bloomFactor;

uniform float u_exposure;

uniform float u_gamma;

layout(binding = 0) uniform sampler2D u_lighting;
layout(binding = 1) uniform sampler2D u_blur;

void main() {
    vec3 result = texture(u_lighting, texCoord).rgb;

    // bloom
    if (u_bloom) {
        result += texture(u_blur, texCoord).rgb * u_bloomFactor;
    }

    // hdr
    if (u_exposure > 0) {
        result = vec3(1.0) - exp(-result * u_exposure);
    }

    result = pow(result, vec3(1.0 / u_gamma));

    fragColor = vec4(result, 1.0f);
}
