#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Common/Base.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace sd {

class SD_API RenderTarget {
   public:
    RenderTarget();

    ~RenderTarget() = default;

    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;

    void init();

    void addTexture(const Ref<Texture> &texture);

    int getWidth() const;

    int getHeight() const;

    void resize(int width, int height);

    void setOrigin(int x, int y);

    void use() const;

    // map coordinate from screen space to clip space
    glm::vec2 mapScreenToClip(const glm::vec2 &pos);
    // map coordinate from clip space to screen space
    glm::vec2 mapClipToScreen(const glm::vec2 &pos);

    Ref<Framebuffer> getFramebuffer();

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
