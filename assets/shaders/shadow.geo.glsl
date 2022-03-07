#version 450 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

#include light.glsl

void main() {          
	for (int i = 0; i < 3; ++i) {
		gl_Position = u_light_matrix[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  
