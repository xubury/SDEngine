#include "Renderer/RenderTarget.hpp"
#include "Renderer/Renderer.hpp"
#include "Utils/Log.hpp"

namespace sd {
RenderTarget::RenderTarget() : m_x(0), m_y(0), m_width(0), m_height(0) {}

void RenderTarget::init() {
    m_framebuffer = Framebuffer::create();
    std::vector<uint32_t> colors;
    for (const auto &texture : m_textures) {
        bool isColor = m_framebuffer->attachTexture(texture.get());
        if (isColor) {
            colors.push_back(colors.size());
        }
    }
    m_framebuffer->setDrawable(colors);
}

void RenderTarget::addTexture(const Ref<Texture> &texture) {
    if (m_textures.empty()) {
        resize(texture->getWidth(), texture->getHeight());
    }
    if (getWidth() == texture->getWidth() &&
        getHeight() == texture->getHeight()) {
        m_textures.push_back(texture);
    } else {
        SD_CORE_WARN("Tried to add texture with diffrent dim!");
    }
}

int RenderTarget::getWidth() const { return m_width; }

int RenderTarget::getHeight() const { return m_height; }

void RenderTarget::resize(int width, int height) {
    m_width = width;
    m_height = height;
    for (Ref<Texture> texture : m_textures) {
        texture->setPixels(width, height, nullptr);
    }
}

void RenderTarget::setOrigin(int x, int y) {
    m_x = x;
    m_y = y;
}

void RenderTarget::use() const {
    Renderer::setFramebuffer(m_framebuffer.get());
    Renderer::setViewport(m_x, m_y, m_width, m_height);
}

glm::vec2 RenderTarget::mapScreenToClip(const glm::vec2 &pos) {
    glm::vec2 view;
    view.x = -1.f + 2.f * (pos.x - m_x) / m_width;
    view.y = 1.f - 2.f * (pos.y - m_y) / m_height;
    return view;
}

glm::vec2 RenderTarget::mapClipToScreen(const glm::vec2 &pos) {
    glm::vec2 screen;
    screen.x = (pos.x + 1.f) * m_width / 2.f + m_x;
    screen.y = (1.f - pos.y) * m_height / 2.f + m_y;
    return screen;
}

}  // namespace sd
