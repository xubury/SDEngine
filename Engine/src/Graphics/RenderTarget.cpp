#include "Graphics/RenderTarget.hpp"

namespace SD {

RenderTarget::RenderTarget(int width, int height)
    : m_width(width), m_height(height) {}

void RenderTarget::CreateFramebuffer() {
    m_framebuffer = Framebuffer::Create();
    m_drawables.clear();

    for (const auto &spec : m_texture_specs) {
        m_framebuffer->AttachTexture(Texture::Create(m_width, m_height, spec));
        if (spec.format != DataFormat::DEPTH &&
            spec.format != DataFormat::DEPTH_STENCIL) {
            m_drawables.push_back(m_drawables.size());
        }
    }
    for (const auto &spec : m_renderbuffer_specs) {
        m_framebuffer->AttachRenderbuffer(
            Renderbuffer::Create(m_width, m_height, spec));
        if (spec.format != DataFormat::DEPTH &&
            spec.format != DataFormat::DEPTH_STENCIL) {
            m_drawables.push_back(m_drawables.size());
        }
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

int RenderTarget::GetWidth() const { return m_width; }

int RenderTarget::GetHeight() const { return m_height; }

void RenderTarget::SetSize(int width, int height) {
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
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
