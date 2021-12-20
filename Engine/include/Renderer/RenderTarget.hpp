#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utility/Base.hpp"
#include "Renderer/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace SD {

class SD_RENDERER_API RenderTarget {
   public:
    RenderTarget(int width, int height);

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
    void Resize(int width, int height);

    Viewport GetViewport() const { return m_viewport; }
    void SetViewport(const Viewport &viewport) { m_viewport = viewport; }

   private:
    int m_width;
    int m_height;
    Ref<Framebuffer> m_framebuffer;

    std::vector<TextureSpec> m_texture_specs;
    std::vector<RenderbufferSpec> m_renderbuffer_specs;

    Viewport m_viewport;
};

}  // namespace SD

#endif /* SD_RENDER_TARGET_HPP */
