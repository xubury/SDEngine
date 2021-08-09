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
              void *data);

    ~GLTexture();

    uint32_t getId() const override;

    bool equals(const Texture &other) const override;

    void bind() const override;
    void unbind() const override;

    void setSlot(uint32_t slot) const override;

    void setPixels(int width, int height, void *data) override;
    void setBorderColor(const void *color) override;
    void setWrap(TextureWrap wrap) override;
    void setFilter(TextureFilter filter) override;
    void setMipmapFilter(TextureMipmapFilter mipmapFilter) override;

    void genareteMipmap() const override;

    void setTextureData(Texture *source, int xOffset, int yOffset, int width,
                        int height, int mipmap) override;

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
