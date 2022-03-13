#ifndef SD_RENDERBUFFER_HPP
#define SD_RENDERBUFFER_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

class SD_GRAPHICS_API Renderbuffer : public Resource {
   public:
    static Ref<Renderbuffer> Create(int32_t width, int32_t height,
                                    int8_t samples, DataFormat format);

    int32_t GetWidth() const { return m_width; }
    int32_t GetHeight() const { return m_height; }
    int32_t GetSamples() const { return m_samples; }

    DataFormat GetFormat() const { return m_format; }

   protected:
    Renderbuffer(int32_t width, int32_t height, int32_t samples,
                 DataFormat format);

    int32_t m_width;
    int32_t m_height;
    int32_t m_samples;

    DataFormat m_format;
};

}  // namespace SD

#endif
