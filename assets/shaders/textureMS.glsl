vec4 textureMS(sampler2DMS sampler, vec2 texCoord) {
    vec4 color = vec4(0.0);

    const int samples = textureSamples(sampler);
    const ivec2 uv = ivec2(texCoord * textureSize(sampler));
    for (int i = 0; i < samples; ++i) {
        color += texelFetch(sampler, uv, i);
    }
    color /= float(samples);

    return color;
}
