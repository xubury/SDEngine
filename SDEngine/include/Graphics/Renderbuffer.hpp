#ifndef SD_RENDERBUFFER_HPP
#define SD_RENDERBUFFER_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

struct SD_GRAPHICS_API RenderbufferSpec {
    uint8_t samples;
    DataFormat format;
    DataFormatType format_type;
    RenderbufferSpec(uint8_t samples, DataFormat format,
                     DataFormatType format_type)
        : samples(samples), format(format), format_type(format_type) {}
};

class SD_GRAPHICS_API Renderbuffer : public Resource {
   public:
    static Ref<Renderbuffer> Create(int width, int height,
                                    const RenderbufferSpec &spec);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetSamples() const { return m_samples; }

    DataFormat GetFormat() const { return m_format; }
    DataFormatType GetFormatType() const { return m_format_type; }

   protected:
    Renderbuffer(int width, int height, int samples, DataFormat format,
                 DataFormatType format_type);

    int m_width;
    int m_height;
    int m_samples;

    DataFormat m_format;
    DataFormatType m_format_type;
};

}  // namespace SD

#endif
