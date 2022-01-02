#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include <GL/glew.h>

namespace SD {

#ifdef DEBUG_BUILD
static void OpenGLMessageCallback(GLenum, GLenum, unsigned, GLenum severity,
                                  int, const char *message, const void *) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            SD_CORE_ASSERT(false, message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            // SD_CORE_ASSERT(false, message);
            SD_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_LOW:
            SD_CORE_WARN(message);
            return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            SD_CORE_TRACE(message);
            return;
        default:
            SD_CORE_ASSERT(false, "Unknown severity level!");
    }
}
#endif

GLDevice::GLDevice(Context *context) : Device(context) {
#ifdef DEBUG_BUILD
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    // Depth Test
    Enable(Operation::DEPTH_TEST);

    // Blend
    Enable(Operation::BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Enable(Operation::CULL_FACE);

    if (context->GetMSAA() > 1) {
        Enable(Operation::MULTISAMPLE);
    } else {
        Disable(Operation::MULTISAMPLE);
    }
}

void GLDevice::DrawElements(MeshTopology topology, int count, size_t offset) {
    glDrawElements(Translate(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::DrawArrays(MeshTopology topology, int first, int count) {
    glDrawArrays(Translate(topology), first, count);
}

void GLDevice::SetLineWidth(float width) { glLineWidth(width); }

void GLDevice::SetClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GLDevice::Clear(BufferBitMask bit) {
    glClear(Translate(bit & BufferBitMask::COLOR_BUFFER_BIT) |
            Translate(bit & BufferBitMask::DEPTH_BUFFER_BIT) |
            Translate(bit & BufferBitMask::STENCIL_BUFFER_BIT));
}

void GLDevice::SetViewport(const Viewport &viewport) {
    // opengl define viewport origin at bottom-left
    glViewport(
        viewport.GetLeft(),
        m_context->GetHeight() - viewport.GetHeight() - viewport.GetTop(),
        viewport.GetWidth(), viewport.GetHeight());
}

void GLDevice::SetFramebuffer(Framebuffer *framebuffer) {
    if (framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetId());
        glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
        if (glCheckNamedFramebufferStatus(framebuffer->GetId(),
                                          GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            SD_CORE_ERROR("FrameBuffer is not complete!");
        }
        GLFramebuffer *glfb = dynamic_cast<GLFramebuffer *>(framebuffer);
        const auto &drawables = glfb->GetDrawables();
        glNamedFramebufferDrawBuffers(glfb->GetId(), drawables.size(),
                                      drawables.data());

    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_context->GetWidth(), m_context->GetHeight());
    }
}

void GLDevice::SetPolygonMode(PolygonMode mode, Face face) {
    glPolygonMode(Translate(face), Translate(mode));
}

void GLDevice::SetDepthMask(bool depth_mask) { glDepthMask(depth_mask); }

void GLDevice::Disable(Operation operation) { glDisable(Translate(operation)); }

void GLDevice::Enable(Operation operation) { glEnable(Translate(operation)); }

void GLDevice::SetCullFace(Face face) { glCullFace(Translate(face)); }

void GLDevice::SetDepthfunc(DepthFunc depth_func) {
    glDepthFunc(Translate(depth_func));
}

void GLDevice::ResetShaderState() { glUseProgram(0); }

void GLDevice::DrawBuffer(Framebuffer *fb, int buf) {
    if (fb) {
        glNamedFramebufferDrawBuffer(fb->GetId(), GL_COLOR_ATTACHMENT0 + buf);
    } else {
        glNamedFramebufferDrawBuffer(0, GL_FRONT_LEFT + buf);
    }
}

void GLDevice::DrawBuffers(Framebuffer *fb, int n, const int *buf) {
    std::vector<GLenum> glbuf(n);
    if (fb) {
        std::generate(glbuf.begin(), glbuf.end(), [i = 0, buf]() mutable {
            return buf[i++] + GL_COLOR_ATTACHMENT0;
        });
        glNamedFramebufferDrawBuffers(fb->GetId(), n, glbuf.data());
    } else {
        std::generate(glbuf.begin(), glbuf.end(), [i = 0, buf]() mutable {
            return buf[i++] + GL_FRONT_LEFT;
        });
        glNamedFramebufferDrawBuffers(0, n, glbuf.data());
    }
}

void GLDevice::ReadBuffer(Framebuffer *fb, int buf) {
    if (fb) {
        glNamedFramebufferReadBuffer(fb->GetId(), GL_COLOR_ATTACHMENT0 + buf);
    } else {
        glNamedFramebufferReadBuffer(0, GL_FRONT_LEFT + buf);
    }
}

void GLDevice::BlitFramebuffer(Framebuffer *src, int src_x, int src_y,
                               int src_width, int src_height, Framebuffer *dst,
                               int dst_x, int dst_y, int dst_width,
                               int dst_height, BufferBitMask mask,
                               BlitFilter filter) {
    int src_id = src ? src->GetId() : 0;
    int dst_id = dst ? dst->GetId() : 0;
    GLint gl_mask = Translate(mask & BufferBitMask::COLOR_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::DEPTH_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::STENCIL_BUFFER_BIT);
    GLenum gl_filter = Translate(filter);
    glBlitNamedFramebuffer(src_id, dst_id, src_x, src_y, src_width, src_height,
                           dst_x, dst_y, dst_width, dst_height, gl_mask,
                           gl_filter);
}

const glm::ivec2 GLDevice::GetUVIndex(int index) const {
    const glm::ivec2 UV_INDEX[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
    return UV_INDEX[index];
}

}  // namespace SD
