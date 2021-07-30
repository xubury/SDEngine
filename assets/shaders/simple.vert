#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Camera
{
    mat4 u_projectionView;
};

out vec4 color;

void main() {
    gl_Position = u_projectionView * vec4(aPos, 1.0f);
    color = aColor;
}
