#include "Graphics/RenderTarget.hpp"

namespace SD {

RenderTarget::RenderTarget(int width, int height)
    : m_viewport(0, 0, width, height) {}

void RenderTarget::CreateFramebuffer() {
    m_framebuffer =
        Framebuffer::Create(m_viewport.GetWidth(), m_viewport.GetHeight());

    for (const auto &spec : m_texture_specs) {
        m_framebuffer->Attach(spec);
    }
    for (const auto &spec : m_renderbuffer_specs) {
        m_framebuffer->Attach(spec);
    }
}

void RenderTarget::AddTexture(const TextureSpec &spec) {
    m_texture_specs.push_back(spec);
}

void RenderTarget::AddRenderbuffer(const RenderbufferSpec &spec) {
    m_renderbuffer_specs.push_back(spec);
}

void RenderTarget::Clear() {
    m_framebuffer = nullptr;
    m_texture_specs.clear();
    m_renderbuffer_specs.clear();
}

int RenderTarget::GetWidth() const { return m_viewport.GetWidth(); }

int RenderTarget::GetHeight() const { return m_viewport.GetHeight(); }

void RenderTarget::SetSize(int left, int top, int width, int height) {
    int old_width = m_viewport.GetWidth();
    int old_height = m_viewport.GetHeight();
    m_viewport.SetSize(left, top, width, height);
    if (old_width != width || old_height != height) {
        CreateFramebuffer();
    }
}

const Framebuffer *RenderTarget::GetFramebuffer() const {
    return m_framebuffer.get();
}

Framebuffer *RenderTarget::GetFramebuffer() { return m_framebuffer.get(); }

Texture *RenderTarget::GetTexture(uint32_t attachmentId) const {
    return m_framebuffer->GetTexture(attachmentId);
}

}  // namespace SD
