#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

GLFramebuffer::GLFramebuffer() { glCreateFramebuffers(1, &m_id); }

GLFramebuffer::~GLFramebuffer() { glDeleteFramebuffers(1, &m_id); }

void GLFramebuffer::Attach(Texture &texture, int attachment, int level)
{
    GLenum gl_attachment;
    switch (texture.GetFormat()) {
        case DataFormat::Depth24:
            gl_attachment = GL_DEPTH_ATTACHMENT;
            break;
        case DataFormat::Stencil8:
            gl_attachment = GL_STENCIL_ATTACHMENT;
            break;
        case DataFormat::Depth24Stencil8:
            gl_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        default:
            gl_attachment = GL_COLOR_ATTACHMENT0 + attachment;
            if (static_cast<int>(m_drawables.size()) <= attachment) {
                m_drawables.push_back(gl_attachment);
                m_textures.push_back(&texture);
            }
            else {
                m_drawables[attachment] = gl_attachment;
                m_textures[attachment] = &texture;
            }
            break;
    }
    glNamedFramebufferTexture(m_id, gl_attachment, texture.Handle(), level);
}

void GLFramebuffer::Attach(Renderbuffer &buffer, int attachment)
{
    GLenum gl_attachment;
    switch (buffer.GetFormat()) {
        case DataFormat::Depth24:
            gl_attachment = GL_DEPTH_ATTACHMENT;
            break;
        case DataFormat::Stencil8:
            gl_attachment = GL_STENCIL_ATTACHMENT;
            break;
        case DataFormat::Depth24Stencil8:
            gl_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
        default:
            gl_attachment = GL_COLOR_ATTACHMENT0 + attachment;
            if (static_cast<int>(m_drawables.size()) <= attachment) {
                m_drawables.push_back(gl_attachment);
            }
            else {
                m_drawables[attachment] = gl_attachment;
            }
            break;
    }
    glNamedFramebufferRenderbuffer(m_id, gl_attachment, GL_RENDERBUFFER,
                                   buffer.Handle());
}

void GLFramebuffer::Prepare()
{
    glNamedFramebufferDrawBuffers(m_id, m_drawables.size(), m_drawables.data());
}

void GLFramebuffer::ClearDepth(const float depth)
{
    glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &depth);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id, const int *value)
{
    glClearNamedFramebufferiv(m_id, GL_COLOR, attachment_id, value);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id,
                                    const uint32_t *value)
{
    glClearNamedFramebufferuiv(m_id, GL_COLOR, attachment_id, value);
}

void GLFramebuffer::ClearAttachment(uint32_t attachment_id, const float *value)
{
    glClearNamedFramebufferfv(m_id, GL_COLOR, attachment_id, value);
}

}  // namespace SD
