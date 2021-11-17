#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utility/Base.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace SD {

class SD_API RenderTarget {
   public:
    RenderTarget();
    RenderTarget(int x, int y, int width, int height);

    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;
    RenderTarget(RenderTarget &&) = default;
    RenderTarget &operator=(RenderTarget &&) = default;

    ~RenderTarget() = default;

    void CreateFramebuffer();
    Framebuffer *GetFramebuffer();
    const Framebuffer *GetFramebuffer() const;

    void AddTexture(const Ref<Texture> &texture);
    Texture *GetTexture(uint32_t attachmentId = 0) const;
    void Clear();

    void SetOrigin(int x, int y);
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;
    void Resize(int width, int height);

    // map coordinate from screen space to clip space
    glm::vec2 MapScreenToClip(const glm::vec2 &pos);
    // map coordinate from clip space to screen space
    glm::vec2 MapClipToScreen(const glm::vec2 &pos);

   private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    std::vector<Ref<Texture>> m_textures;
    Ref<Framebuffer> m_framebuffer;
};

}  // namespace SD

#endif /* SD_RENDER_TARGET_HPP */
