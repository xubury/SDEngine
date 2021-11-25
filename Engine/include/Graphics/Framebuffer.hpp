#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

#include <vector>

namespace SD {

class Texture;

class SD_API Framebuffer {
   public:
    static Ref<Framebuffer> Create();

    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    virtual uint32_t GetId() const = 0;

    virtual bool AttachTexture(const Ref<Texture> &texture) = 0;

    virtual void Clear() = 0;

    virtual void SetDrawable(const std::vector<uint32_t> &color_attchments) = 0;

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

    virtual void Bind() = 0;

    virtual void Resize(int width, int height) = 0;

   protected:
    Framebuffer() = default;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
