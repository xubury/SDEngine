#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"

namespace sd {

GLFramebuffer::GLFramebuffer() : m_id(0), m_textureCnt(0) {
    glCreateFramebuffers(1, &m_id);
}

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

bool GLFramebuffer::attachTexture(const Ref<Texture> &texture) {
    Ref<GLTexture> glTexture = std::static_pointer_cast<GLTexture>(texture);
    bool isColor = false;
    if (glTexture) {
        GLenum attachment = 0;
        switch (glTexture->getFormat()) {
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
                attachment = GL_COLOR_ATTACHMENT0 + m_textureCnt++;
                break;
        }
        m_attachments.emplace_back(attachment, glTexture);
        glNamedFramebufferTexture(m_id, attachment, glTexture->getId(), 0);

    } else {
        SD_CORE_ERROR("Mismatched API!");
    }

    return isColor;
}

void GLFramebuffer::clear() {
    m_textureCnt = 0;
    m_attachments.clear();
}

void GLFramebuffer::setDrawable(const std::vector<uint32_t> &colorAttachments) {
    if (colorAttachments.empty()) {
        glNamedFramebufferDrawBuffer(m_id, GL_NONE);
    } else {
        std::vector<GLenum> glAttachments;
        for (const auto i : colorAttachments) {
            glAttachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glNamedFramebufferDrawBuffers(m_id, glAttachments.size(),
                                      glAttachments.data());
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SD_CORE_ERROR("FrameBuffer is not complete!");
    }
}

void GLFramebuffer::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }

void GLFramebuffer::unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void GLFramebuffer::readPixels(uint32_t attachmentId, int level, int x, int y,
                               int z, int w, int h, int d, size_t size,
                               void *data) const {
    auto glTexture = m_attachments.at(attachmentId).second;
    glTexture->readPixels(level, x, y, z, w, h, d, size, data);
}

void GLFramebuffer::clearDepth(const float depth) {
    glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &depth);
}

void GLFramebuffer::clearAttachment(uint32_t attachmentId, const int *value) {
    glClearNamedFramebufferiv(m_id, GL_COLOR, attachmentId, value);
}

void GLFramebuffer::clearAttachment(uint32_t attachmentId,
                                    const uint32_t *value) {
    glClearNamedFramebufferuiv(m_id, GL_COLOR, attachmentId, value);
}

void GLFramebuffer::clearAttachment(uint32_t attachmentId, const float *value) {
    glClearNamedFramebufferfv(m_id, GL_COLOR, attachmentId, value);
}

Texture *GLFramebuffer::getTexture(uint32_t attachmentId) {
    return m_attachments[attachmentId].second.get();
}

void GLFramebuffer::copyTo(Framebuffer *other, uint32_t index,
                           BufferBitMask mask, TextureFilter filter) const {
    const GLFramebuffer *glFb = dynamic_cast<const GLFramebuffer *>(other);
    int targetId = glFb ? glFb->m_id : 0;
    GLenum glMask = translate(mask & BufferBitMask::COLOR_BUFFER_BIT) |
                    translate(mask & BufferBitMask::DEPTH_BUFFER_BIT) |
                    translate(mask & BufferBitMask::STENCIL_BUFFER_BIT);
    GLenum glFilter = translate(filter);
    const auto &[attachment, texture] = m_attachments[index];
    glNamedFramebufferReadBuffer(m_id, attachment);
    glNamedFramebufferDrawBuffer(targetId, glFb ? attachment : GL_BACK);
    glBlitNamedFramebuffer(m_id, targetId, 0, 0, texture->getWidth(),
                           texture->getHeight(), 0, 0, texture->getWidth(),
                           texture->getHeight(), glMask, glFilter);
}

void GLFramebuffer::resize(int width, int height) {
    for (auto &[attachment, texture] : m_attachments) {
        texture->setPixels(width, height, nullptr);
    }
}

}  // namespace sd
