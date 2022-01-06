#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

GLFramebuffer::GLFramebuffer(int width, int height)
    : m_id(0), m_width(width), m_height(height), m_texture_cnt(0) {
    glCreateFramebuffers(1, &m_id);
}

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

void GLFramebuffer::Attach(const TextureSpec &spec) {
    auto texture = Texture::Create(m_width, m_height, spec);
    GLenum attachment = 0;
    switch (texture->GetFormat()) {
        case DataFormat::DEPTH:
            attachment = GL_DEPTH_ATTACHMENT;
            break;
        case DataFormat::DEPTH_STENCIL:
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        case DataFormat::ALPHA:
        case DataFormat::RED:
        case DataFormat::RG:
        case DataFormat::RGB:
        case DataFormat::RGBA:
            attachment = GL_COLOR_ATTACHMENT0 + m_texture_cnt++;
            m_drawables.push_back(attachment);
            break;
    }
    m_attachments.emplace_back(texture);
    glNamedFramebufferTexture(m_id, attachment, texture->GetId(), 0);
}

void GLFramebuffer::Attach(const RenderbufferSpec &spec) {
    auto renderbuffer = Renderbuffer::Create(m_width, m_height, spec);
    GLenum attachment = 0;
    switch (renderbuffer->GetFormat()) {
        case DataFormat::DEPTH:
            attachment = GL_DEPTH_ATTACHMENT;
            break;
        case DataFormat::DEPTH_STENCIL:
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        case DataFormat::ALPHA:
        case DataFormat::RED:
        case DataFormat::RG:
        case DataFormat::RGB:
        case DataFormat::RGBA:
            attachment = GL_COLOR_ATTACHMENT0 + m_texture_cnt++;
            m_drawables.push_back(attachment);
            break;
    }
    m_attachments.emplace_back(renderbuffer);
    glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER,
                                   renderbuffer->GetId());
}

void GLFramebuffer::ReadPixels(uint32_t attachment_id, int level, int x, int y,
                               int z, int w, int h, int d, size_t size,
                               void *data) const {
    GetTexture(attachment_id)->ReadPixels(level, x, y, z, w, h, d, size, data);
}

void GLFramebuffer::ClearDepth(const float depth) {
    glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &depth);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id, const int *value) {
    glClearNamedFramebufferiv(m_id, GL_COLOR, attachment_id, value);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id,
                                    const uint32_t *value) {
    glClearNamedFramebufferuiv(m_id, GL_COLOR, attachment_id, value);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id,
                                    const float *value) {
    glClearNamedFramebufferfv(m_id, GL_COLOR, attachment_id, value);
}

Texture *GLFramebuffer::GetTexture(uint32_t attachment_id) {
    return static_cast<Texture *>(m_attachments[attachment_id].get());
}

const Texture *GLFramebuffer::GetTexture(uint32_t attachment_id) const {
    return static_cast<const Texture *>(m_attachments[attachment_id].get());
}
}  // namespace SD
