#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include <vector>

namespace sd {

class Texture;

class SD_API Framebuffer {
   public:
    static Ref<Framebuffer> create();

    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    virtual void init() = 0;

    virtual bool attachTexture(const Ref<Texture> &texture) = 0;

    virtual void clear() = 0;

    virtual void setDrawable(const std::vector<uint32_t> &colorAttachments) = 0;

    virtual int readPixels(uint32_t attachmentId, int x, int y) const = 0;

    virtual void clearAttachment(uint32_t attachmentId, const void *value) = 0;

    virtual Texture *getTexture(uint32_t attachmentId = 0) = 0;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void copyFrom(const Framebuffer *other, uint32_t index,
                          BufferBitMask mask, TextureFilter filter) = 0;

    virtual void resize(int width, int height) = 0;

   protected:
    Framebuffer() = default;
};

}  // namespace sd

#endif /* SD_FRAME_BUFFER_HPP */
