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

    virtual uint32_t getId() const = 0;

    virtual bool attachTexture(const Ref<Texture> &texture) = 0;

    virtual void clear() = 0;

    virtual void setDrawable(const std::vector<uint32_t> &colorAttachments) = 0;

    virtual void readPixels(uint32_t attachmentId, int level, int x, int y,
                            int z, int w, int h, int d, size_t size,
                            void *data) const = 0;

    virtual void clearDepth(const float value = 1) = 0;

    virtual void clearAttachment(uint32_t attachmentId, const int *value) = 0;
    virtual void clearAttachment(uint32_t attachmentId,
                                 const uint32_t *value) = 0;
    virtual void clearAttachment(uint32_t attachmentId, const float *value) = 0;

    virtual Texture *getTexture(uint32_t attachmentId = 0) = 0;

    virtual void bind() = 0;

    virtual void resize(int width, int height) = 0;

   protected:
    Framebuffer() = default;
};

}  // namespace sd

#endif /* SD_FRAME_BUFFER_HPP */
