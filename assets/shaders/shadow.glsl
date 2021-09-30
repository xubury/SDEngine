#shader vertex
#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_biTangent;

out float depth;

uniform mat4 u_model;
uniform mat4 u_projectionView;

void main() {
    vec3 fragPos = (u_model * vec4(a_pos, 1.0f)).xyz;
    gl_Position = u_projectionView * vec4(fragPos, 1.0f);
    depth = gl_Position.z / gl_Position.w;
    depth = smoothstep(0.f, 1.0f, depth);
}


#shader fragment
#version 450 core

out vec4 fragColor;

in float depth;

void main() {
    fragColor = vec4(vec3(depth), 1.0f);
}
