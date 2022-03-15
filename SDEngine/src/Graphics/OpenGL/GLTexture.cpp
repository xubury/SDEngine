#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLenum GetFormatType(DataFormat format) {
    switch (format) {
        case DataFormat::ALPHA8:
        case DataFormat::ALPHA16F:
        case DataFormat::ALPHA32F:
        case DataFormat::R8:
        case DataFormat::R16F:
        case DataFormat::R32F:
            return GL_RED;
        case DataFormat::ALPHA32I:
        case DataFormat::ALPHA32UI:
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
        case DataFormat::DEPTH24:
            return GL_DEPTH;
        case DataFormat::STENCIL8:
            return GL_STENCIL_INDEX;
        case DataFormat::DEPTH24_STENCIL8:
            return GL_DEPTH_STENCIL;
    }
    return 0;
}

GLenum GetDataType(DataFormat format) {
    switch (format) {
        case DataFormat::ALPHA8:
        case DataFormat::R8:
        case DataFormat::RG8:
        case DataFormat::RGB8:
        case DataFormat::RGBA8:
        case DataFormat::STENCIL8:
            return GL_UNSIGNED_BYTE;
        case DataFormat::ALPHA32I:
        case DataFormat::R32I:
        case DataFormat::RG32I:
        case DataFormat::RGB32I:
        case DataFormat::RGBA32I:
            return GL_INT;
        case DataFormat::ALPHA32UI:
        case DataFormat::R32UI:
        case DataFormat::RG32UI:
        case DataFormat::RGB32UI:
        case DataFormat::RGBA32UI:
            return GL_UNSIGNED_INT;
        case DataFormat::ALPHA16F:
        case DataFormat::R16F:
        case DataFormat::RG16F:
        case DataFormat::RGB16F:
        case DataFormat::RGBA16F:
        case DataFormat::ALPHA32F:
        case DataFormat::R32F:
        case DataFormat::RG32F:
        case DataFormat::RGB32F:
        case DataFormat::RGBA32F:
        case DataFormat::DEPTH24:
        case DataFormat::DEPTH24_STENCIL8:
            return GL_FLOAT;
    }
    return 0;
}

GLTexture::GLTexture(int width, int height, int depth, MultiSampleLevel samples,
                     TextureType type, DataFormat format, TextureWrap wrap,
                     TextureMinFilter min_filter, TextureMagFilter mag_filter)
    : Texture(width, height, depth, samples, type, format, wrap, min_filter,
              mag_filter),
      gl_format(0),
      gl_format_type(0) {
    gl_format = GetFormatType(m_format);
    gl_format_type = GetDataType(m_format);

    Allocate();

    if (m_type != TextureType::TEX_2D_MULTISAMPLE) {
        SetWrap(m_wrap);
        SetMinFilter(m_min_filter);
        SetMagFilter(m_mag_filter);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &m_id); }

void GLTexture::Allocate() {
    GLenum gl_type = Translate(m_type);
    GLenum gl_sized_format = Translate(m_format);
    glCreateTextures(gl_type, 1, &m_id);

    if (m_format == DataFormat::ALPHA8 || m_format == DataFormat::ALPHA32I ||
        m_format == DataFormat::ALPHA32UI || m_format == DataFormat::ALPHA16F ||
        m_format == DataFormat::ALPHA32F) {
        const GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    switch (m_type) {
        case TextureType::TEX_2D:
        case TextureType::TEX_CUBE: {
            glTextureStorage2D(m_id, m_mipmap_levels, gl_sized_format, m_width,
                               m_height);
        } break;
        case TextureType::TEX_2D_MULTISAMPLE:
            glTextureStorage2DMultisample(m_id, static_cast<GLsizei>(m_samples),
                                          gl_sized_format, m_width, m_height,
                                          GL_TRUE);
            break;
        case TextureType::TEX_2D_ARRAY:
            glTextureStorage3D(m_id, m_mipmap_levels, gl_sized_format, m_width,
                               m_height, m_depth);
            break;
        case TextureType::TEX_3D:
            glTextureStorage3D(m_id, m_mipmap_levels, gl_sized_format, m_width,
                               m_height, m_depth);
            break;
    }
}

void GLTexture::SetSlot(uint32_t slot) const { glBindTextureUnit(slot, m_id); }

void GLTexture::SetPixels(int x, int y, int z, int width, int height, int depth,
                          const void *data) {
    switch (m_type) {
        case TextureType::TEX_2D:
            glTextureSubImage2D(m_id, 0, x, y, width, height, gl_format,
                                gl_format_type, data);
            break;
        case TextureType::TEX_2D_MULTISAMPLE:
            SD_CORE_ASSERT(false,
                           "TEX_2D_MULTISAMPLE is not allowed to set pixels!");
            break;
        case TextureType::TEX_2D_ARRAY:
        case TextureType::TEX_3D:
        case TextureType::TEX_CUBE:
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
    GLint gl_min_filter = Translate(m_min_filter);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, gl_min_filter);
}

void GLTexture::ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                           size_t size, void *data) const {
    glGetTextureSubImage(m_id, level, x, y, z, w, h, d, gl_format,
                         gl_format_type, size, data);
}

}  // namespace SD
