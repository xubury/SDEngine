#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Renderbuffer.hpp"

#include <vector>

namespace SD {

enum class AttachmentType {
    TEXTURE_2D = 0,
    TEXTURE_2D_ARRAY = 1,
    RENDERBUFFER = 2
};

struct AttachmentDescription {
    AttachmentType type;
    DataFormat format;
    MultiSampleLevel samples;
};

struct FramebufferCreateInfo {
    int32_t width;
    int32_t height;
    int32_t depth;
    std::vector<AttachmentDescription> attachments;
};

class SD_GRAPHICS_API Framebuffer : public Resource {
   public:
    static Ref<Framebuffer> Create(const FramebufferCreateInfo &info);
    Framebuffer(const FramebufferCreateInfo &info);
    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    int GetWidth() const { return m_info.width; };
    int GetHeight() const { return m_info.height; };

    void Resize(int32_t width, int32_t height, int32_t depth = 1);

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
    virtual void SetupAttachments() = 0;
    virtual void DestoryAttachments() = 0;
    FramebufferCreateInfo m_info;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
