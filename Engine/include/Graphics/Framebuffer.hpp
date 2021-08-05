#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utils/Base.hpp"
#include "Graphics/Graphics.hpp"
#include <vector>

namespace sd {

class Texture;

class Framebuffer {
   public:
    static Ref<Framebuffer> create();

    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    virtual void init() = 0;

    virtual bool attachTexture(Texture *) = 0;

    virtual void setDrawable(const std::vector<uint32_t> &colorAttachments) = 0;

    virtual int readPixels(uint32_t attachmentId, int x, int y) const = 0;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void copy(const Framebuffer *other, BufferBit bufferbit,
                      TextureFilter filter) = 0;

   protected:
    Framebuffer() = default;
};

}  // namespace sd

#endif /* SD_FRAME_BUFFER_HPP */
