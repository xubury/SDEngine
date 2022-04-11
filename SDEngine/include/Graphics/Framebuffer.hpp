#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Renderbuffer.hpp"

#include <vector>

namespace SD {

class SD_GRAPHICS_API Framebuffer {
   public:
    static Ref<Framebuffer> Create();
    Framebuffer() = default;
    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    virtual uint32_t Handle() const = 0;
    virtual void Attach(Texture &texture, int attachment, int level) = 0;
    virtual void Attach(Renderbuffer &buffer, int attachment) = 0;
    virtual Texture *GetAttachment(int attachment) = 0;
    virtual void Prepare() = 0;

    virtual void ClearDepth(const float value = 1) = 0;

    virtual void ClearAttachment(uint32_t attachment_id, const int *value) = 0;
    virtual void ClearAttachment(uint32_t attachment_id,
                                 const uint32_t *value) = 0;
    virtual void ClearAttachment(uint32_t attachment_id,
                                 const float *value) = 0;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
