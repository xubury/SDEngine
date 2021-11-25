#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLTexture::GLTexture(int width, int height, int samples, TextureType type,
                     TextureFormat format, TextureFormatType format_type,
                     TextureWrap wrap, TextureFilter filter,
                     TextureMipmapFilter mipmap_filter, const void *data)
    : Texture(width, height, samples, type, format, format_type, wrap, filter,
              mipmap_filter),
      gl_id(0),
      gl_type(0),
      gl_internal_format(0),
      gl_format(0),
      gl_format_type(0) {
    gl_type = Translate(m_type);
    gl_internal_format = TranslateInternalFormat(m_format, m_format_type);
    gl_format = TranslateFormat(m_format, m_format_type);
    gl_format_type = Translate(m_format_type);

    glCreateTextures(gl_type, 1, &gl_id);

    if (m_format == TextureFormat::ALPHA) {
        const GLint swizzle_mask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(gl_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    SetPixels(m_width, m_height, data, 0);
    if (m_type != TextureType::TEX_2D_MULTISAMPLE) {
        SetWrap(m_wrap);
        SetFilter(m_filter);
        SetMipmapFilter(m_mipmap_filter);
        glGenerateTextureMipmap(gl_id);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &gl_id); }

uint32_t GLTexture::GetId() const { return gl_id; }

void GLTexture::Bind() const { glBindTexture(gl_type, gl_id); }

void GLTexture::Unbind() const { glBindTexture(gl_type, 0); }

void GLTexture::SetSlot(uint32_t slot) const { glBindTextureUnit(slot, gl_id); }

void GLTexture::SetPixels(int width, int height, const void *data,
                          uint8_t face) {
    m_width = width;
    m_height = height;

    Bind();
    if (m_format == TextureFormat::RED || m_format == TextureFormat::ALPHA) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    switch (m_type) {
        case TextureType::TEX_2D:
            glTexImage2D(gl_type, 0, gl_internal_format, m_width, m_height, 0,
                         gl_format, gl_format_type, data);
            break;
        case TextureType::TEX_2D_MULTISAMPLE:
            glTexImage2DMultisample(gl_type, m_samples, gl_internal_format,
                                    m_width, m_height, GL_TRUE);
            break;
        case TextureType::TEX_3D:
            glTexImage3D(gl_type, 0, gl_internal_format, m_width, m_height,
                         m_height, 0, gl_format, gl_format_type, data);
            break;
        case TextureType::TEX_CUBE:
            GLenum glFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
            glTexImage2D(glFace, 0, gl_internal_format, m_width, m_height, 0,
                         gl_format, gl_format_type, data);
            break;
    }
    Unbind();
}

void GLTexture::SetBorderColor(const void *color) {
    switch (m_format_type) {
        case TextureFormatType::FLOAT:
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

void GLTexture::SetFilter(TextureFilter filter) {
    m_filter = filter;

    GLint glFilter = Translate(m_filter);
    glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, glFilter);
}

void GLTexture::SetMipmapFilter(TextureMipmapFilter mipmap_filter) {
    m_mipmap_filter = mipmap_filter;
    GLint glMipmapFilter = Translate(m_mipmap_filter);
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
