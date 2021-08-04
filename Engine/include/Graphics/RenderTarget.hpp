#ifndef SD_RENDER_TARGET_HPP
#define SD_RENDER_TARGET_HPP

#include "Utils/Base.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Framebuffer.hpp"
#include <vector>

namespace sd {

class RenderTarget {
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

    void use() const;

    Ref<Framebuffer> m_framebuffer;
   private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    std::vector<Ref<Texture>> m_textures;
};

}  // namespace sd

#endif /* SD_RENDER_TARGET_HPP */
