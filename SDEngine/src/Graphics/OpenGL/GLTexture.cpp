#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLTexture::GLTexture(int width, int height, int depth, int8_t samples,
                     TextureType type, DataFormat format,
                     DataFormatType format_type, TextureWrap wrap,
                     TextureMinFilter min_filter, TextureMagFilter mag_filter)
    : Texture(width, height, depth, samples, type, format, format_type, wrap,
              min_filter, mag_filter),
      gl_type(0),
      gl_internal_format(0),
      gl_format(0),
      gl_format_type(0) {
    gl_type = Translate(m_type);
    gl_internal_format = TranslateInternalFormat(m_format, m_format_type);
    gl_format = TranslateFormat(m_format, m_format_type);
    gl_format_type = Translate(m_format_type);

    Allocate();

    if (m_type != TextureType::TEX_2D_MULTISAMPLE) {
        SetWrap(m_wrap);
        SetMinFilter(m_min_filter);
        SetMagFilter(m_mag_filter);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &m_id); }

void GLTexture::Allocate() {
    glCreateTextures(gl_type, 1, &m_id);

    if (m_format == DataFormat::ALPHA) {
        const GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    switch (m_type) {
        case TextureType::TEX_2D:
        case TextureType::TEX_CUBE: {
            glTextureStorage2D(m_id, m_mipmap_levels, gl_internal_format,
                               m_width, m_height);
        } break;
        case TextureType::TEX_2D_MULTISAMPLE:
            glTextureStorage2DMultisample(m_id, m_samples, gl_internal_format,
                                          m_width, m_height, GL_TRUE);
            break;
        case TextureType::TEX_2D_ARRAY:
            glTextureStorage3D(m_id, m_mipmap_levels, gl_internal_format,
                               m_width, m_height, m_depth);
            break;
        case TextureType::TEX_3D:
            glTextureStorage3D(m_id, m_mipmap_levels, gl_internal_format,
                               m_width, m_height, m_depth);
            break;
    }
}

// void GLTexture::Bind() const { glBindTexture(gl_type, m_id); }

// void GLTexture::Unbind() const { glBindTexture(gl_type, 0); }

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
    switch (m_format_type) {
        case DataFormatType::FLOAT32:
        case DataFormatType::FLOAT16:
            glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, (float *)color);
            break;
        case DataFormatType::INT:
        case DataFormatType::UINT:
        case DataFormatType::UBYTE:
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
