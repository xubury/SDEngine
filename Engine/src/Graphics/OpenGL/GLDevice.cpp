#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/Texture.hpp"
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

GLDevice::GLDevice() {
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
    glViewport(viewport.GetLeft(), viewport.GetBottom(), viewport.GetWidth(),
               viewport.GetHeight());
}

void GLDevice::SetFramebuffer(Framebuffer *framebuffer) {
    if (framebuffer) {
        framebuffer->Bind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void GLDevice::BlitFramebuffer(Framebuffer *src, uint32_t src_attachment,
                               Framebuffer *dst, uint32_t dst_attachment,
                               BufferBitMask mask, BlitFilter filter) {
    int src_id = src ? src->GetId() : 0;
    int dst_id = dst ? dst->GetId() : 0;
    GLint gl_mask = Translate(mask & BufferBitMask::COLOR_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::DEPTH_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::STENCIL_BUFFER_BIT);
    GLenum gl_filter = Translate(filter);
    GLenum src_mode = GL_COLOR_ATTACHMENT0 + src_attachment;
    GLenum dst_mode = GL_COLOR_ATTACHMENT0 + dst_attachment;
    Texture *texture =
        src ? src->GetTexture(src_attachment) : dst->GetTexture(dst_attachment);
    SD_CORE_ASSERT(texture != nullptr, "Invalid framebuffer");
    glNamedFramebufferReadBuffer(src_id, src ? src_mode : GL_BACK);
    glNamedFramebufferDrawBuffer(dst_id, dst ? dst_mode : GL_BACK);
    glBlitNamedFramebuffer(src_id, dst_id, 0, 0, texture->GetWidth(),
                           texture->GetHeight(), 0, 0, texture->GetWidth(),
                           texture->GetHeight(), gl_mask, gl_filter);
}

const glm::ivec2 GLDevice::GetUVIndex(int index) const {
    const glm::ivec2 UV_INDEX[4] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
    return UV_INDEX[index];
}

}  // namespace SD
