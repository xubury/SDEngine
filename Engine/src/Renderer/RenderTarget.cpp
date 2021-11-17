#include "Renderer/RenderTarget.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

RenderTarget::RenderTarget() : m_x(0), m_y(0), m_width(0), m_height(0) {}

RenderTarget::RenderTarget(int x, int y, int width, int height)
    : m_x(x), m_y(y), m_width(width), m_height(height) {}

void RenderTarget::CreateFramebuffer() {
    m_framebuffer = Framebuffer::Create();
    std::vector<uint32_t> colors;
    for (const auto &texture : m_textures) {
        bool isColor = m_framebuffer->AttachTexture(texture);
        if (isColor) {
            colors.push_back(colors.size());
        }
    }
    m_framebuffer->SetDrawable(colors);
}

void RenderTarget::AddTexture(const Ref<Texture> &texture) {
    if (m_textures.empty()) {
        Resize(texture->GetWidth(), texture->GetHeight());
    }
    if (GetWidth() == texture->GetWidth() &&
        GetHeight() == texture->GetHeight()) {
        m_textures.push_back(texture);
    } else {
        SD_CORE_WARN("Tried to add texture with diffrent dim!");
    }
}

void RenderTarget::Clear() {
    if (m_framebuffer) m_framebuffer->Clear();
    m_textures.clear();
}

int RenderTarget::GetX() const { return m_x; }

int RenderTarget::GetY() const { return m_y; }

int RenderTarget::GetWidth() const { return m_width; }

int RenderTarget::GetHeight() const { return m_height; }

void RenderTarget::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    for (Ref<Texture> texture : m_textures) {
        texture->SetPixels(width, height, nullptr);
    }
}

void RenderTarget::SetOrigin(int x, int y) {
    m_x = x;
    m_y = y;
}

glm::vec2 RenderTarget::MapScreenToClip(const glm::vec2 &pos) {
    glm::vec2 clip;
    clip.x = -1.f + 2.f * (pos.x - m_x) / m_width;
    clip.y = 1.f - 2.f * (pos.y - m_y) / m_height;
    return clip;
}

glm::vec2 RenderTarget::MapClipToScreen(const glm::vec2 &pos) {
    glm::vec2 screen;
    screen.x = (pos.x + 1.f) * m_width / 2.f + m_x;
    screen.y = (1.f - pos.y) * m_height / 2.f + m_y;
    return screen;
}

const Framebuffer *RenderTarget::GetFramebuffer() const {
    return m_framebuffer.get();
}

Framebuffer *RenderTarget::GetFramebuffer() { return m_framebuffer.get(); }

Texture *RenderTarget::GetTexture(uint32_t attachmentId) const {
    return m_framebuffer->GetTexture(attachmentId);
}

}  // namespace SD
