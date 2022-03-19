#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLenum GetFormatType(DataFormat format) {
    switch (format) {
        case DataFormat::Alpha8:
        case DataFormat::Alpha16F:
        case DataFormat::Alpha32F:
        case DataFormat::R8:
        case DataFormat::R16F:
        case DataFormat::R32F:
            return GL_RED;
        case DataFormat::Alpha32I:
        case DataFormat::Alpha32UI:
        case DataFormat::R32I:
        case DataFormat::R32UI:
            return GL_RED_INTEGER;
        case DataFormat::RG8:
        case DataFormat::RG16F:
        case DataFormat::RG32F:
            return GL_RG;
        case DataFormat::RG32I:
        case DataFormat::RG32UI:
            return GL_RG_INTEGER;
        case DataFormat::RGB8:
        case DataFormat::RGB16F:
        case DataFormat::RGB32F:
            return GL_RGB;
        case DataFormat::RGB32I:
        case DataFormat::RGB32UI:
            return GL_RGB_INTEGER;
        case DataFormat::RGBA8:
        case DataFormat::RGBA16F:
        case DataFormat::RGBA32F:
            return GL_RGBA;
        case DataFormat::RGBA32I:
        case DataFormat::RGBA32UI:
            return GL_RGBA_INTEGER;
        case DataFormat::Depth24:
            return GL_DEPTH;
        case DataFormat::Stencil8:
            return GL_STENCIL_INDEX;
        case DataFormat::Depth24Stencil8:
            return GL_DEPTH_STENCIL;
    }
    return 0;
}

GLenum GetDataType(DataFormat format) {
    switch (format) {
        case DataFormat::Alpha8:
        case DataFormat::R8:
        case DataFormat::RG8:
        case DataFormat::RGB8:
        case DataFormat::RGBA8:
        case DataFormat::Stencil8:
            return GL_UNSIGNED_BYTE;
        case DataFormat::Alpha32I:
        case DataFormat::R32I:
        case DataFormat::RG32I:
        case DataFormat::RGB32I:
        case DataFormat::RGBA32I:
            return GL_INT;
        case DataFormat::Alpha32UI:
        case DataFormat::R32UI:
        case DataFormat::RG32UI:
        case DataFormat::RGB32UI:
        case DataFormat::RGBA32UI:
            return GL_UNSIGNED_INT;
        case DataFormat::Alpha16F:
        case DataFormat::R16F:
        case DataFormat::RG16F:
        case DataFormat::RGB16F:
        case DataFormat::RGBA16F:
        case DataFormat::Alpha32F:
        case DataFormat::R32F:
        case DataFormat::RG32F:
        case DataFormat::RGB32F:
        case DataFormat::RGBA32F:
        case DataFormat::Depth24:
        case DataFormat::Depth24Stencil8:
            return GL_FLOAT;
    }
    return 0;
}

GLTexture::GLTexture(int width, int height, int depth, MultiSampleLevel samples,
                     TextureType type, DataFormat format, TextureWrap wrap,
                     TextureMinFilter min_filter, MipmapMode mode,
                     TextureMagFilter mag_filter)
    : Texture(width, height, depth, samples, type, format, wrap, min_filter,
              mode, mag_filter),
      gl_format(0),
      gl_format_type(0) {
    gl_format = GetFormatType(m_format);
    gl_format_type = GetDataType(m_format);

    Allocate();

    if (m_samples == MultiSampleLevel::X1) {
        SetWrap(m_wrap);
        SetMinFilter(m_min_filter);
        SetMagFilter(m_mag_filter);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &m_id); }

void GLTexture::Allocate() {
    int32_t dimension = (m_width != 0) + (m_height != 0) + (m_depth != 0);
    gl_type = Translate(m_type, dimension, m_samples);
    GLenum gl_sized_format = Translate(m_format);
    glCreateTextures(gl_type, 1, &m_id);

    if (m_format == DataFormat::Alpha8 || m_format == DataFormat::Alpha32I ||
        m_format == DataFormat::Alpha32UI || m_format == DataFormat::Alpha16F ||
        m_format == DataFormat::Alpha32F) {
        const GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    switch (gl_type) {
        default:
            SD_CORE_ERROR("Unimplemented Allocate() for texture type: {}",
                          gl_type);
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
            glTextureStorage2D(m_id, m_mipmap_levels, gl_sized_format, m_width,
                               m_height);
            break;
        case GL_TEXTURE_2D_MULTISAMPLE:
            glTextureStorage2DMultisample(m_id, static_cast<GLsizei>(m_samples),
                                          gl_sized_format, m_width, m_height,
                                          GL_TRUE);
            break;
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            glTextureStorage3D(m_id, m_mipmap_levels, gl_sized_format, m_width,
                               m_height, m_depth);
            break;
    }
}

void GLTexture::SetPixels(int x, int y, int z, int width, int height, int depth,
                          const void *data) {
    switch (gl_type) {
        default:
            SD_CORE_ERROR("Unimplemented SetPixels() for texture type: {}",
                          gl_type);
            break;
        case GL_TEXTURE_2D:
            glTextureSubImage2D(m_id, 0, x, y, width, height, gl_format,
                                gl_format_type, data);
            break;
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_CUBE_MAP:
            glTextureSubImage3D(m_id, 0, x, y, z, width, height, depth,
                                gl_format, gl_format_type, data);
            break;
    }
    glGenerateTextureMipmap(m_id);
}

void GLTexture::SetBorderColor(const void *color) {
    switch (gl_format_type) {
        case GL_FLOAT:
            glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, (float *)color);
            break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_BYTE:
            glTextureParameteriv(m_id, GL_TEXTURE_BORDER_COLOR, (int *)color);
            break;
    }
}

void GLTexture::SetWrap(TextureWrap wrap) {
    m_wrap = wrap;

    GLint gl_wrap = Translate(m_wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, gl_wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, gl_wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, gl_wrap);
}

void GLTexture::SetMagFilter(TextureMagFilter filter) {
    m_mag_filter = filter;

    GLint gl_filter = Translate(m_mag_filter);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, gl_filter);
}

void GLTexture::SetMinFilter(TextureMinFilter min_filter) {
    m_min_filter = min_filter;
    GLint gl_min_filter = Translate(m_min_filter, m_mipmap_mode);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, gl_min_filter);
}

void GLTexture::SetMipmapMode(MipmapMode mode) {
    m_mipmap_mode = mode;
    GLint gl_min_filter = Translate(m_min_filter, m_mipmap_mode);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, gl_min_filter);
}

void GLTexture::ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                           size_t size, void *data) const {
    glGetTextureSubImage(m_id, level, x, y, z, w, h, d, gl_format,
                         gl_format_type, size, data);
}

}  // namespace SD
