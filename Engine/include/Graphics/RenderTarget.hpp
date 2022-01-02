#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace SD {

class SD_GRAPHICS_API RenderTarget {
   public:
    RenderTarget() = default;
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
    void SetSize(int width, int height);

   private:
    int m_width;
    int m_height;
    Ref<Framebuffer> m_framebuffer;

    std::vector<TextureSpec> m_texture_specs;
    std::vector<RenderbufferSpec> m_renderbuffer_specs;
};

}  // namespace SD

#endif /* SD_RENDER_TARGET_HPP */
