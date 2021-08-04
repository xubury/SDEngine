#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace sd {

GLTexture::GLTexture(int width, int height, TextureType type,
                     TextureFormat format, TextureFormatType formatType,
                     TextureWrap wrap, TextureFilter filter,
                     TextureMipmapFilter mipmapFilter, void *data)
    : Texture(width, height, type, format, formatType, wrap, filter,
              mipmapFilter, data),
      gl_id(0),
      gl_type(0),
      gl_iFormat(0),
      gl_format(0),
      gl_formatType(0) {}

GLTexture::~GLTexture() {
    if (gl_id != 0) glDeleteTextures(1, &gl_id);
}

bool GLTexture::equals(const Texture &other) const {
    const GLTexture *ptr = dynamic_cast<const GLTexture *>(&other);
    if (ptr) {
        return gl_id == ptr->gl_id;
    }
    return false;
}

void GLTexture::init() {
    if (gl_id != 0) glDeleteTextures(1, &gl_id);
    glGenTextures(1, &gl_id);

    gl_type = TRANSLATE(m_type);
    gl_iFormat = TRANSLATE(m_format, m_formatType);
    gl_format = TRANSLATE(m_format);
    gl_formatType = TRANSLATE(m_formatType);

    setPixels(m_width, m_height, m_data);
    setWrap(m_wrap);
    setFilter(m_filter);
    setMipmapFilter(m_mipmapFilter);
}

GLuint GLTexture::id() const { return gl_id; }

void GLTexture::bind() const { glBindTexture(gl_type, gl_id); }

void GLTexture::unbind() const { glBindTexture(gl_type, 0); }

void GLTexture::setPixels(int width, int height, void *data) {
    m_width = width;
    m_height = height;
    m_data = data;

    bind();

    switch (m_type) {
        case TextureType::TEX_2D:
            glTexImage2D(gl_type, 0, gl_iFormat, m_width, m_height, 0,
                         gl_format, gl_formatType, data);
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
    glGenerateMipmap(gl_type);
    unbind();
}

void GLTexture::setBorderColor(const void *color) {
    bind();
    switch (m_formatType) {
        case TextureFormatType::FLOAT:
            glTexParameterfv(gl_type, GL_TEXTURE_BORDER_COLOR, (float *)color);
            break;
        case TextureFormatType::UBYTE:
            glTexParameteriv(gl_type, GL_TEXTURE_BORDER_COLOR, (int *)color);
            break;
    }
}

void GLTexture::setWrap(TextureWrap wrap) {
    m_wrap = wrap;

    bind();
    GLint glWrap = TRANSLATE(m_wrap);
    glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, glWrap);
    glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, glWrap);
    glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, glWrap);
}

void GLTexture::setFilter(TextureFilter filter) {
    m_filter = filter;

    bind();
    GLint glFilter = TRANSLATE(m_filter);
    glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, glFilter);
}

void GLTexture::setMipmapFilter(TextureMipmapFilter mipmapFilter) {
    m_mipmapFilter = mipmapFilter;

    bind();
    GLint glMipmapFilter = TRANSLATE(m_mipmapFilter);
    glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, glMipmapFilter);
}

void GLTexture::genareteMipmap() const {
    bind();
    glGenerateMipmap(gl_type);
}

void GLTexture::setTextureData(Texture *source, int xOffset, int yOffset,
                               int width, int height, int mipmap) {
    if (m_data) {
        glTextureSubImage2D(gl_id, mipmap, xOffset, yOffset, width, height,
                            gl_format, gl_formatType, source->getData());
        genareteMipmap();
    }
}

}  // namespace sd
