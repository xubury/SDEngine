#include "Renderer/RenderTarget.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

RenderTarget::RenderTarget(int x, int y, int width, int height)
    : m_x(x),
      m_y(y),
      m_width(width),
      m_height(height),
      m_framebuffer(nullptr) {}

void RenderTarget::CreateFramebuffer() {
    m_framebuffer = Framebuffer::Create();

    std::vector<uint32_t> colors;
    for (const auto &spec : m_texture_specs) {
        m_framebuffer->AttachTexture(Texture::Create(
            m_width, m_height, spec.samples, spec.type, spec.format,
            spec.format_type, spec.wrap, spec.filter, spec.mipmap_filter));
        if (spec.format != TextureFormat::DEPTH &&
            spec.format != TextureFormat::DEPTH_STENCIL) {
            colors.push_back(colors.size());
        }
    }
    m_framebuffer->SetDrawable(colors);
}

void RenderTarget::AddTexture(const TextureSpec &spec) {
    m_texture_specs.push_back(spec);
}

void RenderTarget::Clear() {
    if (m_framebuffer) m_framebuffer->Clear();
    m_texture_specs.clear();
}

int RenderTarget::GetX() const { return m_x; }

int RenderTarget::GetY() const { return m_y; }

int RenderTarget::GetWidth() const { return m_width; }

int RenderTarget::GetHeight() const { return m_height; }

void RenderTarget::Resize(int width, int height) {
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        if (m_framebuffer) CreateFramebuffer();
    }
}

void RenderTarget::SetOrigin(int x, int y) {
    m_x = x;
    m_y = y;
}

const Framebuffer *RenderTarget::GetFramebuffer() const {
    return m_framebuffer.get();
}

Framebuffer *RenderTarget::GetFramebuffer() { return m_framebuffer.get(); }

Texture *RenderTarget::GetTexture(uint32_t attachmentId) const {
    return m_framebuffer->GetTexture(attachmentId);
}

}  // namespace SD
