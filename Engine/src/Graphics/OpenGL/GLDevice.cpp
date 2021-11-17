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

void GLDevice::DrawElements(MeshTopology topology, size_t count,
                            size_t offset) {
    glDrawElements(Translate(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::SetClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GLDevice::Clear(BufferBitMask bit) {
    glClear(Translate(bit & BufferBitMask::COLOR_BUFFER_BIT) |
            Translate(bit & BufferBitMask::DEPTH_BUFFER_BIT) |
            Translate(bit & BufferBitMask::STENCIL_BUFFER_BIT));
}

void GLDevice::SetViewport(int x, int y, int width, int height) {
    // opengl define viewport origin at bottom-left
    glViewport(x, y, width, height);
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

void GLDevice::SetDepthMask(bool depthMask) { glDepthMask(depthMask); }

void GLDevice::Disable(Operation operation) { glDisable(Translate(operation)); }

void GLDevice::Enable(Operation operation) { glEnable(Translate(operation)); }

void GLDevice::SetCullFace(Face cullFace) { glCullFace(Translate(cullFace)); }

void GLDevice::SetDepthfunc(DepthFunc depthFunc) {
    glDepthFunc(Translate(depthFunc));
}

void GLDevice::ResetShaderState() { glUseProgram(0); }

void GLDevice::BlitFramebuffer(Framebuffer *src, uint32_t srcAttachment,
                               Framebuffer *dst, uint32_t dstAttachment,
                               BufferBitMask mask, TextureFilter filter) {
    int srcId = src ? src->GetId() : 0;
    int dstId = dst ? dst->GetId() : 0;
    GLenum glMask = Translate(mask & BufferBitMask::COLOR_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::DEPTH_BUFFER_BIT) |
                    Translate(mask & BufferBitMask::STENCIL_BUFFER_BIT);
    GLenum glFilter = Translate(filter);
    GLenum srcMode = GL_COLOR_ATTACHMENT0 + srcAttachment;
    GLenum dstMode = GL_COLOR_ATTACHMENT0 + dstAttachment;
    Texture *texture =
        src ? src->GetTexture(srcAttachment) : dst->GetTexture(dstAttachment);
    SD_CORE_ASSERT(texture != nullptr, "Invalid framebuffer");
    glNamedFramebufferReadBuffer(srcId, src ? srcMode : GL_BACK);
    glNamedFramebufferDrawBuffer(dstId, dst ? dstMode : GL_BACK);
    glBlitNamedFramebuffer(srcId, dstId, 0, 0, texture->GetWidth(),
                           texture->GetHeight(), 0, 0, texture->GetWidth(),
                           texture->GetHeight(), glMask, glFilter);
}

}  // namespace SD
