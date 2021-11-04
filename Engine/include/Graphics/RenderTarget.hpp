#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utility/Base.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace sd {

class SD_API RenderTarget {
   public:
    RenderTarget();
    RenderTarget(int x, int y, int width, int height);

    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;
    RenderTarget(RenderTarget &&) = default;
    RenderTarget &operator=(RenderTarget &&) = default;

    ~RenderTarget() = default;

    void createFramebuffer();
    Framebuffer *getFramebuffer();
    const Framebuffer *getFramebuffer() const;

    void addTexture(const Ref<Texture> &texture);
    Texture *getTexture(uint32_t attachmentId = 0) const;
    void clear();

    void setOrigin(int x, int y);
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    void resize(int width, int height);

    // map coordinate from screen space to clip space
    glm::vec2 mapScreenToClip(const glm::vec2 &pos);
    // map coordinate from clip space to screen space
    glm::vec2 mapClipToScreen(const glm::vec2 &pos);

   private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    std::vector<Ref<Texture>> m_textures;
    Ref<Framebuffer> m_framebuffer;
};

}  // namespace sd

#endif /* SD_RENDER_TARGET_HPP */
