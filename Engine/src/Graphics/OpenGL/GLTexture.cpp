#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLTexture::GLTexture(int width, int height, int samples, TextureType type,
                     TextureFormat format, TextureFormatType format_type,
                     TextureWrap wrap, TextureMagFilter filter,
                     TextureMinFilter min_filter)
    : Texture(width, height, samples, type, format, format_type, wrap, filter,
              min_filter),
      gl_id(0),
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
        SetMagFilter(m_filter);
        SetMinFilter(m_min_filter);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &gl_id); }

void GLTexture::Allocate() {
    glCreateTextures(gl_type, 1, &gl_id);

    if (m_format == TextureFormat::ALPHA) {
        const GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(gl_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    switch (m_type) {
        case TextureType::TEX_2D:
        case TextureType::TEX_CUBE: {
            glTextureStorage2D(gl_id, m_mipmap_levels, gl_internal_format,
                               m_width, m_height);
        } break;
        case TextureType::TEX_2D_MULTISAMPLE:
            glTextureStorage2DMultisample(gl_id, m_samples, gl_internal_format,
                                          m_width, m_height, GL_TRUE);
            break;
        case TextureType::TEX_3D:
            // FIXME: depth not impl
            glTextureStorage3D(gl_id, m_mipmap_levels, gl_internal_format,
                               m_width, m_height, 0);
            break;
    }
}

uint32_t GLTexture::GetId() const { return gl_id; }

void GLTexture::Bind() const { glBindTexture(gl_type, gl_id); }

void GLTexture::Unbind() const { glBindTexture(gl_type, 0); }

void GLTexture::SetSlot(uint32_t slot) const { glBindTextureUnit(slot, gl_id); }

void GLTexture::SetPixels(int x, int y, int z, size_t width, size_t height,
                          size_t depth, const void *data) {
    switch (m_type) {
        case TextureType::TEX_2D:
            glTextureSubImage2D(gl_id, 0, x, y, width, height, gl_format,
                                gl_format_type, data);
            break;
        case TextureType::TEX_2D_MULTISAMPLE:
            SD_CORE_ASSERT(false,
                           "TEX_2D_MULTISAMPLE is not allowed to set pixels!");
            break;
        case TextureType::TEX_3D:
        case TextureType::TEX_CUBE:
            glTextureSubImage3D(gl_id, 0, x, y, z, width, height, depth,
                                gl_format, gl_format_type, data);
            break;
    }
    if (m_mipmap_levels > 1) glGenerateTextureMipmap(gl_id);
}

void GLTexture::SetBorderColor(const void *color) {
    switch (m_format_type) {
        case TextureFormatType::FLOAT32:
        case TextureFormatType::FLOAT16:
            glTextureParameterfv(gl_id, GL_TEXTURE_BORDER_COLOR,
                                 (float *)color);
            break;
        case TextureFormatType::UINT24_8:
        case TextureFormatType::UINT:
        case TextureFormatType::UBYTE:
            glTextureParameteriv(gl_id, GL_TEXTURE_BORDER_COLOR, (int *)color);
            break;
    }
}

void GLTexture::SetWrap(TextureWrap wrap) {
    m_wrap = wrap;

    GLint glWrap = Translate(m_wrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_R, glWrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_S, glWrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_T, glWrap);
}

void GLTexture::SetMagFilter(TextureMagFilter filter) {
    m_filter = filter;

    GLint glFilter = Translate(m_filter);
    glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, glFilter);
}

void GLTexture::SetMinFilter(TextureMinFilter min_filter) {
    m_min_filter = min_filter;
    GLint glMipmapFilter = Translate(m_min_filter);
    glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, glMipmapFilter);
}

void GLTexture::ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                           size_t size, void *data) const {
    glGetTextureSubImage(gl_id, level, x, y, z, w, h, d, gl_format,
                         gl_format_type, size, data);
}

GLenum GLTexture::GetGLType() const { return gl_type; }

GLenum GLTexture::GetGLFormat() const { return gl_format; }

GLenum GLTexture::GetGLFormatType() const { return gl_format_type; }

}  // namespace SD
