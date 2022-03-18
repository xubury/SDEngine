#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace SD {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, int depth, MultiSampleLevel samples,
              TextureType type, DataFormat format, TextureWrap wrap,
              TextureMinFilter min_filter, MipmapMode mode,
              TextureMagFilter mag_filter);

    ~GLTexture();

    uint32_t GetId() const override { return m_id; }

    void SetPixels(int x, int y, int z, int width, int height, int depth,
                   const void *data) override;
    void SetBorderColor(const void *color) override;
    void SetWrap(TextureWrap wrap) override;
    void SetMagFilter(TextureMagFilter filter) override;
    void SetMinFilter(TextureMinFilter min_filter) override;
    void SetMipmapMode(MipmapMode mode) override;

    void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                    size_t size, void *data) const override;

   private:
    void Allocate();

    GLenum gl_type;
    GLenum gl_format;
    GLenum gl_format_type;
    GLuint m_id;
};

}  // namespace SD

#endif /* SD_GL_TEXTURE_HPP */
