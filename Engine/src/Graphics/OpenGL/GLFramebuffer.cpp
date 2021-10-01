#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTexture.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"

namespace sd {
GLFramebuffer::GLFramebuffer() : m_id(0), m_textureCnt(0) {}

GLFramebuffer::~GLFramebuffer() {
    if (m_id != 0) glDeleteFramebuffers(1, &m_id);
}

void GLFramebuffer::init() {
    if (m_id != 0) glDeleteFramebuffers(1, &m_id);
    glGenFramebuffers(1, &m_id);
}

bool GLFramebuffer::attachTexture(const Ref<Texture> &texture) {
    Ref<GLTexture> glTexture = std::static_pointer_cast<GLTexture>(texture);
    bool isColor = false;
    if (glTexture) {
        GLenum attachment = 0;
        switch (glTexture->getFormat()) {
            case TextureFormat::DEPTH:
                attachment = GL_DEPTH_ATTACHMENT;
                break;
            case TextureFormat::STENCIL:
                attachment = GL_STENCIL_ATTACHMENT;
                break;
            default:
                isColor = true;
                attachment = GL_COLOR_ATTACHMENT0 + m_textureCnt++;
                break;
        }
        m_attachments.emplace_back(attachment, glTexture);
        bind();
        glTexture->bind();
        switch (glTexture->getType()) {
            case TextureType::TEX_2D:
            case TextureType::TEX_2D_MULTISAMPLE:
                glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                                       glTexture->getGLType(),
                                       glTexture->getId(), 0);
                break;
            case TextureType::TEX_3D:
            case TextureType::TEX_CUBE:
                glFramebufferTexture(GL_FRAMEBUFFER, attachment,
                                     glTexture->getId(), 0);
                break;
            default:
                SD_CORE_ERROR(
                    "[GLFramebuffer::attachTexture] Invalid texture attachment "
                    "type!");
        }

        glTexture->unbind();
        unbind();
    } else {
        SD_CORE_ERROR("Mismatched API!");
    }

    return isColor;
}

void GLFramebuffer::setDrawable(const std::vector<uint32_t> &colorAttachments) {
    bind();
    std::vector<GLenum> glAttachments;
    for (const auto i : colorAttachments) {
        glAttachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(glAttachments.size(), glAttachments.data());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SD_CORE_ERROR("FrameBuffer is not complete!");
    }
    unbind();
}

void GLFramebuffer::bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_id); }

void GLFramebuffer::unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

int GLFramebuffer::readPixels(uint32_t attachmentId, int x, int y) const {
    bind();
    GLenum glAttachmentId = m_attachments.at(attachmentId).first;
    glReadBuffer(glAttachmentId);

    int data = 0;
    GLTexture *texture = m_attachments.at(attachmentId).second.get();
    glReadPixels(x, y, 1, 1, texture->getGLFormat(), texture->getGLFormatType(),
                 &data);
    unbind();
    return data;
}

void GLFramebuffer::clearAttachment(uint32_t attachmentId, const void *value) {
    GLTexture *texture = m_attachments[attachmentId].second.get();
    glClearTexImage(texture->getId(), 0, texture->getGLFormat(),
                    texture->getGLFormatType(), value);
}

Texture *GLFramebuffer::getTexture(uint32_t attachmentId) {
    return m_attachments[attachmentId].second.get();
}

void GLFramebuffer::copyFrom(const Framebuffer *other, BufferBit mask,
                             TextureFilter filter) {
    const GLFramebuffer *glFb = dynamic_cast<const GLFramebuffer *>(other);
    if (glFb) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, glFb->m_id);
        GLenum glMask = 0;
        if (mask & BufferBit::COLOR_BUFFER_BIT) {
            glMask |= GL_COLOR_BUFFER_BIT;
        }
        if (mask & BufferBit::DEPTH_BUFFER_BIT) {
            glMask |= GL_DEPTH_BUFFER_BIT;
        }
        if (mask & BufferBit::STENCIL_BUFFER_BIT) {
            glMask |= GL_STENCIL_BUFFER_BIT;
        }
        GLenum glFilter;
        switch (filter) {
            case TextureFilter::LINEAR:
                glFilter = GL_LINEAR;
                break;
            case TextureFilter::NEAREST:
                glFilter = GL_NEAREST;
                break;
        }
        for (const auto &[attachment, texture] : m_attachments) {
            glReadBuffer(attachment);
            glDrawBuffer(attachment);
            glBlitFramebuffer(0, 0, texture->getWidth(), texture->getHeight(),
                              0, 0, texture->getWidth(), texture->getHeight(),
                              glMask, glFilter);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    } else {
        SD_CORE_ERROR("Mismatched API!");
    }
}

void GLFramebuffer::resize(int width, int height) {
    for (auto &[attachment, texture] : m_attachments) {
        texture->setPixels(width, height, nullptr);
    }
}

}  // namespace sd
