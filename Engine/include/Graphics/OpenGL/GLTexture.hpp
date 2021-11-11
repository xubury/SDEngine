#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace SD {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, int samples, TextureType type,
              TextureFormat format, TextureFormatType formatType,
              TextureWrap wrap, TextureFilter filter,
              TextureMipmapFilter mipmapFilter, const void *data);

    ~GLTexture();

    uint32_t getId() const override;

    void bind() const override;
    void unbind() const override;

    void setSlot(uint32_t slot) const override;

    void setPixels(int width, int height, const void *data) override;
    void setBorderColor(const void *color) override;
    void setWrap(TextureWrap wrap) override;
    void setFilter(TextureFilter filter) override;
    void setMipmapFilter(TextureMipmapFilter mipmapFilter) override;

    void readPixels(int level, int x, int y, int z, int w, int h, int d,
                    size_t size, void *data) const override;

    GLenum getGLType() const;
    GLenum getGLFormat() const;
    GLenum getGLFormatType() const;

   private:
    GLuint gl_id;
    GLenum gl_type;
    GLint gl_iFormat;
    GLenum gl_format;
    GLenum gl_formatType;
};

}  // namespace SD

#endif /* SD_GL_TEXTURE_HPP */
