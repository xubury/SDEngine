#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Renderbuffer.hpp"

#include <vector>

namespace SD {

enum AttachmentType { TEXTURE_2D = 0, TEXTURE_2D_ARRAY = 1, RENDERBUFFER = 2 };

struct AttachmentDescription {
    AttachmentType type;
    DataFormat format;
    DataFormatType format_type;
};

class SD_GRAPHICS_API Framebuffer : public Resource {
   public:
    static Ref<Framebuffer> Create(int32_t width, int32_t height, int32_t depth,
                                   int8_t samples);
    Framebuffer(int32_t width, int32_t height, int32_t depth, int8_t samples);
    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    int GetWidth() const { return m_width; };
    int GetHeight() const { return m_height; };

    void Attach(const AttachmentDescription &attchment);

    void Resize(int32_t width, int32_t height, int32_t depth = 1);
    virtual void Setup() = 0;
    virtual void Invalidate() = 0;

    virtual void ReadPixels(uint32_t attachment_id, int level, int x, int y,
                            int z, int w, int h, int d, size_t size,
                            void *data) const = 0;

    virtual void ClearDepth(const float value = 1) = 0;

    virtual void ClearAttachment(uint32_t attachment_id, const int *value) = 0;
    virtual void ClearAttachment(uint32_t attachment_id,
                                 const uint32_t *value) = 0;
    virtual void ClearAttachment(uint32_t attachment_id,
                                 const float *value) = 0;

    virtual Texture *GetTexture(uint32_t attachment_id = 0) = 0;
    virtual const Texture *GetTexture(uint32_t attachment_id = 0) const = 0;

   protected:
    int32_t m_width;
    int32_t m_height;
    int32_t m_depth;
    int8_t m_samples;

    std::vector<AttachmentDescription> m_attachment_descs;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
