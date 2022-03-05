#ifndef SD_FRAME_BUFFER_HPP
#define SD_FRAME_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Renderbuffer.hpp"

#include <vector>

namespace SD {

class SD_GRAPHICS_API Framebuffer : public Resource {
   public:
    static Ref<Framebuffer> Create(int width, int height);

    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    int GetWidth() const { return m_width; };
    int GetHeight() const { return m_height; };

    void Attach(const TextureSpec &spec);
    void Attach(const RenderbufferSpec &renderbuffer);

    virtual void Resize(int width, int height) = 0;
    virtual void Setup() = 0;

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
    Framebuffer(int32_t width, int32_t height)
        : m_width(width), m_height(height) {}

    std::vector<TextureSpec> m_texture_specs;
    std::vector<RenderbufferSpec> m_renderbuffer_specs;
};

}  // namespace SD

#endif /* SD_FRAME_BUFFER_HPP */
