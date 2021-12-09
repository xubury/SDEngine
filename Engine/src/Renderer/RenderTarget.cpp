#include "Renderer/RenderTarget.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

RenderTarget::RenderTarget(int x, int y, int width, int height)
    : m_x(x),
      m_y(y),
      m_width(width),
      m_height(height),
      m_framebuffer(Framebuffer::Create()) {}

void RenderTarget::CreateFramebuffer() {
    std::vector<uint32_t> colors;
    for (const auto &spec : m_texture_specs) {
        m_framebuffer->AttachTexture(Texture::Create(m_width, m_height, spec));
        if (spec.format != DataFormat::DEPTH &&
            spec.format != DataFormat::DEPTH_STENCIL) {
            colors.push_back(colors.size());
        }
    }
    for (const auto &spec : m_renderbuffer_specs) {
        m_framebuffer->AttachRenderbuffer(
            Renderbuffer::Create(m_width, m_height, spec));
        if (spec.format != DataFormat::DEPTH &&
            spec.format != DataFormat::DEPTH_STENCIL) {
            colors.push_back(colors.size());
        }
    }
    m_framebuffer->SetDrawable(colors);
}

void RenderTarget::AddTexture(const TextureSpec &spec) {
    m_texture_specs.push_back(spec);
}

void RenderTarget::AddRenderbuffer(const RenderbufferSpec &spec) {
    m_renderbuffer_specs.push_back(spec);
}

void RenderTarget::Clear() {
    m_framebuffer = Framebuffer::Create();
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
        m_framebuffer = Framebuffer::Create();
        CreateFramebuffer();
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
