#version 450 core

#include camera.glsl
#include light.glsl
#include shadow.glsl

layout(location = 0) out vec3 frag_color;

layout(location = 0) in vec2 in_uv;

uniform LightColor u_light_color;
uniform PointLight u_point_light;
uniform bool u_is_directional;
uniform vec3 u_light_front;

uniform bool u_is_cast_shadow;

uniform sampler2DMS u_lighting;
uniform sampler2DMS u_position;
uniform sampler2DMS u_normal;
uniform sampler2DMS u_albedo;
uniform sampler2DMS u_ambient;
uniform sampler2DMS u_background;
uniform sampler2D u_ssao;
uniform bool u_ssao_state;

void main()
{
    vec3 color = vec3(0);
    const int samples = textureSamples(u_position);
    const float ambient_occlusion = u_ssao_state ? texture(u_ssao, in_uv).r : 1;

    const ivec2 uv = ivec2(in_uv * textureSize(u_position));

    for (int i = 0; i < samples; ++i) {
        vec3 normal = texelFetch(u_normal, uv, i).rgb;
        if (normal != vec3(0)) {
            // Fragment is not a background,
            // calculate the lighting result
            const vec3 pos = texelFetch(u_position, uv, i).rgb;
            const vec3 last = texelFetch(u_lighting, uv, i).rgb;
            const vec4 albedo = texelFetch(u_albedo, uv, i);
            const vec3 ambient =
                texelFetch(u_ambient, uv, i).rgb * ambient_occlusion;
            const vec3 view_dir = normalize(u_view[3].xyz - pos);
            vec3 result = vec3(0);
            if (u_is_directional) {
                // DirectionalLight
                float shadow = 0.f;
                const vec3 light_dir = normalize(-u_light_front);
                if (u_is_cast_shadow)
                {
                    shadow = DirShadowCalculation(light_dir, pos, normal, u_view);
                }
                result = CalcDirLight(u_light_front, u_light_color, normal, 
                                   view_dir, ambient, albedo, shadow);
            }
            else {
                // PointLight
                float shadow = 0.f;
                if (u_is_cast_shadow)
                {
                    shadow = CubeShadowCalculation(pos, u_point_light.pos, u_view[3].xyz);
                }
                result = CalcPointLight(u_point_light, u_light_color, pos, normal, 
                                        view_dir, ambient, albedo, shadow);

            }
            color += last + result;
        }
        else {
            // Fragment is background, don't calculate light,
            // just add the last lighting result or the background color.
            color += texelFetch(u_background, uv, i).rgb;
        }
    }
    frag_color = color / samples;
}
