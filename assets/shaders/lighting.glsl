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

#include shaders/camera.glsl
#include shaders/light.glsl

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 in_texCoord;

uniform Light u_light;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;
uniform sampler2D u_ambient;

void main() {
    vec3 fragPos = texture(u_position, in_texCoord).rgb;
    vec3 normal = texture(u_normal, in_texCoord).rgb;
    normal = normalize(normal);
    vec3 diffuse = texture(u_albedo, in_texCoord).rgb;
    vec3 specular = vec3(texture(u_albedo, in_texCoord).a);
    vec3 ambient = texture(u_ambient, in_texCoord).rgb;

    vec3 viewDir = normalize(u_viewPos - fragPos);

    vec3 result = calculateLight(u_light, fragPos, normal, viewDir, ambient, diffuse, specular);

    fragColor = vec4(result, 1.0f);
}
