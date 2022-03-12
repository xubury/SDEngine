#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

GLFramebuffer::GLFramebuffer() { glCreateFramebuffers(1, &m_id); }

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

void GLFramebuffer::Clear() {
    m_attachments.clear();
    m_drawables.clear();
    glInvalidateNamedFramebufferData(m_id, m_attachment_types.size(),
                                     m_attachment_types.data());
}

void GLFramebuffer::Resize(int width, int height) {
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        for (auto &spec : m_texture_specs) {
            spec.width = width;
            spec.height = height;
        }
        for (auto &spec : m_renderbuffer_specs) {
            spec.width = width;
            spec.height = height;
        }
        Clear();
        Setup();
    }
}

void GLFramebuffer::Setup() {
    int drawable_cnt = 0;
    for (const auto &spec : m_texture_specs) {
        m_width = spec.width;
        m_height = spec.height;
        auto texture = Texture::Create(spec);
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
                attachment = GL_COLOR_ATTACHMENT0 + drawable_cnt++;
                m_drawables.push_back(attachment);
                break;
        }
        m_attachment_types.emplace_back(attachment);
        m_attachments.emplace_back(texture);
        glNamedFramebufferTexture(m_id, attachment, texture->GetId(), 0);
    }
    for (const auto &spec : m_renderbuffer_specs) {
        m_width = spec.width;
        m_height = spec.height;
        auto renderbuffer = Renderbuffer::Create(spec);
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
                attachment = GL_COLOR_ATTACHMENT0 + drawable_cnt++;
                m_drawables.push_back(attachment);
                break;
        }
        m_attachment_types.emplace_back(attachment);
        m_attachments.emplace_back(renderbuffer);
        glNamedFramebufferRenderbuffer(m_id, attachment, GL_RENDERBUFFER,
                                       renderbuffer->GetId());
    }
    SD_CORE_ASSERT(
        glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
        "Framebuffer is incomplete!");
    glNamedFramebufferDrawBuffers(GetId(), m_drawables.size(),
                                  m_drawables.data());
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
