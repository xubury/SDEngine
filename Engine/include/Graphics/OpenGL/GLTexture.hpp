#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace SD {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, int samples, TextureType type,
              TextureFormat format, TextureFormatType format_type,
              TextureWrap wrap, TextureMagFilter filter,
              TextureMinFilter min_filter);

    ~GLTexture();

    uint32_t GetId() const override;

    void Bind() const override;
    void Unbind() const override;

    void SetSlot(uint32_t slot) const override;

    void SetPixels(int x, int y, int z, int width, int height, int depth,
                   const void *data) override;
    void SetBorderColor(const void *color) override;
    void SetWrap(TextureWrap wrap) override;
    void SetMagFilter(TextureMagFilter filter) override;
    void SetMinFilter(TextureMinFilter min_filter) override;

    void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                    size_t size, void *data) const override;

   private:
    void Allocate();

    GLuint gl_id;
    GLenum gl_type;
    GLint gl_internal_format;
    GLenum gl_format;
    GLenum gl_format_type;
};

}  // namespace SD

#endif /* SD_GL_TEXTURE_HPP */
