#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

GLFramebuffer::GLFramebuffer(int32_t width, int32_t height, int32_t depth,
                             int8_t samples)
    : Framebuffer(width, height, depth, samples) {
    glCreateFramebuffers(1, &m_id);
}

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

void GLFramebuffer::Invalidate() {
    glInvalidateNamedFramebufferData(m_id, m_attachment_types.size(),
                                     m_attachment_types.data());
    m_attachments.clear();
    m_attachment_types.clear();
}

void GLFramebuffer::Setup() {
    std::vector<GLenum> drawables;
    for (const auto &spec : m_attachment_descs) {
        GLenum attachment = 0;
        switch (spec.format) {
            case DataFormat::DEPTH24:
                attachment = GL_DEPTH_ATTACHMENT;
                break;
            case DataFormat::STENCIL8:
                attachment = GL_STENCIL_ATTACHMENT;
                break;
            case DataFormat::DEPTH24_STENCIL8:
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
                break;
            default:
                attachment = GL_COLOR_ATTACHMENT0 + drawables.size();
                drawables.emplace_back(attachment);
                break;
        }
        m_attachment_types.emplace_back(attachment);

        switch (spec.type) {
            case AttachmentType::TEXTURE_2D: {
                auto texture = Texture::Create(
                    m_width, m_height, m_depth, m_samples,
                    m_samples > 1 ? TextureType::TEX_2D_MULTISAMPLE
                                  : TextureType::TEX_2D,
                    spec.format);
                m_attachments.emplace_back(texture);
                glNamedFramebufferTexture(m_id, attachment, texture->GetId(),
                                          0);
            } break;
            case AttachmentType::TEXTURE_2D_ARRAY: {
                auto texture =
                    Texture::Create(m_width, m_height, m_depth, m_samples,
                                    TextureType::TEX_2D_ARRAY, spec.format);
                m_attachments.emplace_back(texture);
                glNamedFramebufferTexture(m_id, attachment, texture->GetId(),
                                          0);
            } break;
            case AttachmentType::RENDERBUFFER: {
                auto renderbuffer = Renderbuffer::Create(
                    m_width, m_height, m_samples, spec.format);
                m_attachments.emplace_back(renderbuffer);
                glNamedFramebufferRenderbuffer(
                    m_id, attachment, GL_RENDERBUFFER, renderbuffer->GetId());
            } break;
        }
    }
    SD_CORE_ASSERT(glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) ==
                       GL_FRAMEBUFFER_COMPLETE,
                   "Framebuffer is incomplete!");
    glNamedFramebufferDrawBuffers(m_id, drawables.size(), drawables.data());
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
