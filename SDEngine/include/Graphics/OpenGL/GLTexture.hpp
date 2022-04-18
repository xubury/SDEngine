#ifndef SD_GL_TEXTURE_HPP
#define SD_GL_TEXTURE_HPP

#include "Graphics/Texture.hpp"
#include <GL/glew.h>

namespace SD {

class GLTexture : public Texture {
   public:
    GLTexture(int width, int height, int depth, MultiSampleLevel samples,
              TextureType type, DataFormat format, TextureParameter parameter,
              int32_t mipmap_levels);

    ~GLTexture();

    uint32_t Handle() const override { return m_id; }

    void SetPixels(int x, int y, int z, int width, int height, int depth,
                   const void *data) override;
    void SetBorderColor(const void *color) override;
    void SetWrap(TextureWrap wrap) override;
    void SetMagFilter(TextureMagFilter filter) override;
    void SetMinFilter(TextureMinFilter min_filter) override;
    void SetBaseLevel(int level) override;
    void SetMaxLevel(int level) override;
    void GenerateMipmap() override;

    void ReadPixels(int level, int x, int y, int z, int w, int h, int d,
                    size_t size, void *data) const override;

    int GetWidth() const override { return m_width; };
    int GetHeight() const override { return m_height; };
    MultiSampleLevel GetSamples() const override { return m_samples; };

    TextureType GetType() const override { return m_type; };
    DataFormat GetFormat() const override { return m_format; };

    TextureWrap GetWrap() const override { return m_params.wrap; }

    TextureMinFilter GetMinFilter() const override
    {
        return m_params.min_filter;
    }

    TextureMagFilter GetMagFilter() const override
    {
        return m_params.mag_filter;
    }

    MipmapMode GetMipmapMode() const override { return m_params.mipmap; }

    int GetMipmapLevels() const override { return m_mipmap_levels; }

   private:
    void Allocate();
    int m_width;
    int m_height;
    int m_depth;
    MultiSampleLevel m_samples;
    int m_mipmap_levels;

    TextureType m_type;
    DataFormat m_format;
    TextureParameter m_params;

    GLenum gl_type;
    GLenum gl_format;
    GLenum gl_format_type;
    GLuint m_id;
};

}  // namespace SD

#endif /* SD_GL_TEXTURE_HPP */
