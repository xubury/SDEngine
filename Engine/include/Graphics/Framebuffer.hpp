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

    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    virtual uint32_t GetId() const = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual void AttachTexture(const Ref<Texture> &texture) = 0;

    virtual void AttachRenderbuffer(const Ref<Renderbuffer> &renderbuffer) = 0;

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
    Framebuffer() = default;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
