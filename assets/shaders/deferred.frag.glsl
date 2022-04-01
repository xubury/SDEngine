#version 450 core

#include camera.glsl
#include light.glsl
#include shadow.glsl

layout(location = 0) out vec3 frag_color;

layout(location = 0) in vec2 in_uv;

uniform Light u_light;
uniform vec3 u_light_position;
uniform vec3 u_light_front;

uniform sampler2DMS u_lighting;
uniform sampler2DMS u_position;
uniform sampler2DMS u_normal;
uniform sampler2DMS u_height;
uniform sampler2DMS u_tangent;
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
            vec3 tangent = texelFetch(u_tangent, uv, i).rgb;
            tangent = normalize(tangent - dot(tangent, normal) * normal);
            const vec3 last = texelFetch(u_lighting, uv, i).rgb;
            const vec4 albedo = texelFetch(u_albedo, uv, i);
            const vec3 ambient =
                texelFetch(u_ambient, uv, i).rgb * ambient_occlusion;
            const vec3 height = texelFetch(u_height, uv, i).rgb;
            const vec3 view_dir = normalize(u_view[3].xyz - pos);
            const vec3 light_dir = u_light.is_directional
                                       ? normalize(-u_light_front)
                                       : normalize(u_light_position - pos);

            if (height != vec3(0)) {
                mat3 TBN =
                    transpose(mat3(tangent, cross(normal, tangent), normal));
                const float shadow =
                    u_light.is_cast_shadow
                        ? ShadowCalculation(TBN * light_dir, pos, height,
                                            u_view)
                        : 0;
                color += last + CalculateLight(u_light, TBN * u_light_position,
                                               TBN * u_light_front, TBN * pos,
                                               height, TBN * view_dir, ambient,
                                               albedo, shadow);
            }
            else {
                const float shadow =
                    u_light.is_cast_shadow
                        ? ShadowCalculation(light_dir, pos, normal, u_view)
                        : 0;
                color +=
                    last + CalculateLight(u_light, u_light_position,
                                          u_light_front, pos, normal, view_dir,
                                          ambient, albedo, shadow);
            }
        }
        else {
            // Fragment is background, don't calculate light,
            // just add the last lighting result or the background color.
            color += texelFetch(u_background, uv, i).rgb;
        }
    }
    frag_color = color / samples;
}
