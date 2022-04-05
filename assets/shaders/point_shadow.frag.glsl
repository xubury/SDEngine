#version 450 core
layout(location = 0) in vec4 frag_pos;

uniform vec3 u_light_pos;
uniform float u_far_z;

void main()
{
    float dist = length(frag_pos.xyz - u_light_pos);
    
    // map to [0;1] range by dividing by u_far_z
    dist = dist / u_far_z;
    
    // write this as modified depth
    gl_FragDepth = dist;
}
