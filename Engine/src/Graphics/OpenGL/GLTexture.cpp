#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Assert.hpp"

namespace sd {

GLTexture::GLTexture(int width, int height, int samples, TextureType type,
                     TextureFormat format, TextureFormatType formatType,
                     TextureWrap wrap, TextureFilter filter,
                     TextureMipmapFilter mipmapFilter, const void *data)
    : Texture(width, height, samples, type, format, formatType, wrap, filter,
              mipmapFilter),
      gl_id(0),
      gl_type(0),
      gl_iFormat(0),
      gl_format(0),
      gl_formatType(0) {
    gl_type = translate(m_type);
    gl_iFormat = translateInternalFormat(m_format, m_formatType);
    gl_format = translateFormat(m_format, m_formatType);
    gl_formatType = translate(m_formatType);

    glCreateTextures(gl_type, 1, &gl_id);

    if (m_format == TextureFormat::ALPHA) {
        GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTextureParameteriv(gl_id, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    setPixels(m_width, m_height, data);
    if (m_type != TextureType::TEX_2D_MULTISAMPLE) {
        setWrap(m_wrap);
        setFilter(m_filter);
        setMipmapFilter(m_mipmapFilter);
        glGenerateTextureMipmap(gl_id);
    }
}

GLTexture::~GLTexture() { glDeleteTextures(1, &gl_id); }

uint32_t GLTexture::getId() const { return gl_id; }

void GLTexture::bind() const { glBindTexture(gl_type, gl_id); }

void GLTexture::unbind() const { glBindTexture(gl_type, 0); }

void GLTexture::setSlot(uint32_t slot) const { glBindTextureUnit(slot, gl_id); }

void GLTexture::setPixels(int width, int height, const void *data) {
    m_width = width;
    m_height = height;

    bind();
    if (m_format == TextureFormat::RED || m_format == TextureFormat::ALPHA) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    switch (m_type) {
        case TextureType::TEX_2D:
            glTexImage2D(gl_type, 0, gl_iFormat, m_width, m_height, 0,
                         gl_format, gl_formatType, data);
            break;
        case TextureType::TEX_2D_MULTISAMPLE:
            glTexImage2DMultisample(gl_type, m_samples, gl_iFormat, m_width,
                                    m_height, GL_TRUE);
            break;
        case TextureType::TEX_3D:
            glTexImage3D(gl_type, 0, gl_iFormat, m_width, m_height, m_height, 0,
                         gl_format, gl_formatType, data);
            break;
        case TextureType::TEX_CUBE:
            for (uint8_t i = 0; i < 6; i++) {
                GLenum glFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                glTexImage2D(glFace, 0, gl_iFormat, m_width, m_height, 0,
                             gl_format, gl_formatType, data);
            }
            break;
    }
    unbind();
}

void GLTexture::setBorderColor(const void *color) {
    switch (m_formatType) {
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

void GLTexture::setWrap(TextureWrap wrap) {
    m_wrap = wrap;

    GLint glWrap = translate(m_wrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_R, glWrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_S, glWrap);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_T, glWrap);
}

void GLTexture::setFilter(TextureFilter filter) {
    m_filter = filter;

    GLint glFilter = translate(m_filter);
    glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, glFilter);
}

void GLTexture::setMipmapFilter(TextureMipmapFilter mipmapFilter) {
    m_mipmapFilter = mipmapFilter;
    GLint glMipmapFilter = translate(m_mipmapFilter);
    glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, glMipmapFilter);
}

void GLTexture::readPixels(int level, int x, int y, int z, int w, int h, int d,
                           size_t size, void *data) const {
    glGetTextureSubImage(gl_id, level, x, y, z, w, h, d, gl_format,
                         gl_formatType, size, data);
}

GLenum GLTexture::getGLType() const { return gl_type; }

GLenum GLTexture::getGLFormat() const { return gl_format; }

GLenum GLTexture::getGLFormatType() const { return gl_formatType; }

}  // namespace sd
