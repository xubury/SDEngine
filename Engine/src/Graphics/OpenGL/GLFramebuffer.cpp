#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

GLFramebuffer::GLFramebuffer() : m_id(0), m_texture_cnt(0) {
    glCreateFramebuffers(1, &m_id);
}

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

bool GLFramebuffer::AttachTexture(const Ref<Texture> &texture) {
    bool isColor = false;
    GLenum attachment = 0;
    switch (texture->GetFormat()) {
        case TextureFormat::DEPTH:
            attachment = GL_DEPTH_ATTACHMENT;
            break;
        case TextureFormat::DEPTH_STENCIL:
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        case TextureFormat::ALPHA:
        case TextureFormat::RED:
        case TextureFormat::RG:
        case TextureFormat::RGB:
        case TextureFormat::RGBA:
            isColor = true;
            attachment = GL_COLOR_ATTACHMENT0 + m_texture_cnt++;
            break;
    }
    m_attachments.emplace_back(attachment, texture);
    glNamedFramebufferTexture(m_id, attachment, texture->GetId(), 0);

    return isColor;
}

void GLFramebuffer::Clear() {
    m_texture_cnt = 0;
    m_attachments.clear();
}

void GLFramebuffer::SetDrawable(
    const std::vector<uint32_t> &color_attachments) {
    if (color_attachments.empty()) {
        glNamedFramebufferDrawBuffer(m_id, GL_NONE);
    } else {
        std::vector<GLenum> gl_attchments;
        for (const auto i : color_attachments) {
            gl_attchments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glNamedFramebufferDrawBuffers(m_id, gl_attchments.size(),
                                      gl_attchments.data());
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SD_CORE_ERROR("FrameBuffer is not complete!");
    }
}

void GLFramebuffer::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }

void GLFramebuffer::ReadPixels(uint32_t attachment_id, int level, int x, int y,
                               int z, int w, int h, int d, size_t size,
                               void *data) const {
    auto texture = m_attachments.at(attachment_id).second;
    texture->ReadPixels(level, x, y, z, w, h, d, size, data);
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
    return m_attachments[attachment_id].second.get();
}

void GLFramebuffer::Resize(int width, int height) {
    for (auto &[attachment, texture] : m_attachments) {
        texture->SetPixels(width, height, nullptr);
    }
}

}  // namespace SD
