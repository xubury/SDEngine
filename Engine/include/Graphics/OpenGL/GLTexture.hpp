#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace sd {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, TextureType type, TextureFormat format,
              TextureFormatType formatType, TextureWrap wrap,
              TextureFilter filter, TextureMipmapFilter mipmapFilter,
              const void *data);

    ~GLTexture();

    uint32_t id() const override;
    void bind() const override;
    void unbind() const override;

    void setPixels(int width, int height, const void *data) override;
    void setBorderColor(const void *color) override;
    void setWrap(TextureWrap wrap) override;
    void setFilter(TextureFilter filter) override;
    void setMipmapFilter(TextureMipmapFilter mipmapFilter) override;

    void genareteMipmap() const override;

   protected:
    void init() override;

   private:
    GLuint gl_id;
    GLenum gl_type;
    GLint gl_iFormat;
    GLenum gl_format;
    GLenum gl_formatType;
};

}  // namespace sd

#endif /* SD_GL_TEXTURE_HPP */
