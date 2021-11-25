#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace SD {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, int samples, TextureType type,
              TextureFormat format, TextureFormatType format_type,
              TextureWrap wrap, TextureFilter filter,
              TextureMipmapFilter mipmap_filter, const void *data);

    ~GLTexture();

    uint32_t GetId() const override;

    void Bind() const override;
    void Unbind() const override;

    void SetSlot(uint32_t slot) const override;

    void SetPixels(int width, int height, const void *data,
                   uint8_t face) override;
    void SetBorderColor(const void *color) override;
    void SetWrap(TextureWrap wrap) override;
    void SetFilter(TextureFilter filter) override;
    void SetMipmapFilter(TextureMipmapFilter mipmap_filter) override;

    void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                    size_t size, void *data) const override;

    GLenum GetGLType() const;
    GLenum GetGLFormat() const;
    GLenum GetGLFormatType() const;

   private:
    GLuint gl_id;
    GLenum gl_type;
    GLint gl_internal_format;
    GLenum gl_format;
    GLenum gl_format_type;
};

}  // namespace SD

#endif /* SD_GL_TEXTURE_HPP */
