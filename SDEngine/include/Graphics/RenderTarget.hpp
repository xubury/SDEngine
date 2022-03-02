#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Viewport.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace SD {

class SD_GRAPHICS_API RenderTarget {
   public:
    RenderTarget() = default;
    RenderTarget(const Viewport &viewport);

    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;
    RenderTarget(RenderTarget &&) = default;
    RenderTarget &operator=(RenderTarget &&) = default;

    ~RenderTarget() = default;

    void CreateFramebuffer();
    Framebuffer *GetFramebuffer();
    const Framebuffer *GetFramebuffer() const;

    void AddTexture(const TextureSpec &spec);
    Texture *GetTexture(uint32_t attachmentId = 0) const;

    void AddRenderbuffer(const RenderbufferSpec &spec);

    void Clear();

    int GetWidth() const;
    int GetHeight() const;
    void SetSize(int left, int top, int width, int height);

    const Viewport &GetViewport() const { return m_viewport; }
    Viewport &GetViewport() { return m_viewport; }
    void SetViewport(const Viewport &viewport) { m_viewport = viewport; }

   private:
    Viewport m_viewport;
    Ref<Framebuffer> m_framebuffer;

    std::vector<TextureSpec> m_texture_specs;
    std::vector<RenderbufferSpec> m_renderbuffer_specs;
};

}  // namespace SD

#endif /* SD_RENDER_TARGET_HPP */
