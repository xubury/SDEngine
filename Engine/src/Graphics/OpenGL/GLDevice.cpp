#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include "Utility/Assert.hpp"
#include <GL/glew.h>

namespace sd {

#ifdef DEBUG_BUILD
static void OpenGLMessageCallback(GLenum, GLenum, unsigned, GLenum severity,
                                  int, const char *message, const void *) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            SD_CORE_ASSERT(false, message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            SD_CORE_ASSERT(false, message);
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
    enable(Operation::DEPTH_TEST);

    // Blend
    enable(Operation::BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    enable(Operation::CULL_FACE);
}

void GLDevice::drawElements(MeshTopology topology, size_t count,
                            size_t offset) {
    glDrawElements(translate(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GLDevice::clear(BufferBitMask bit) {
    glClear(translate(bit & BufferBitMask::COLOR_BUFFER_BIT) |
            translate(bit & BufferBitMask::DEPTH_BUFFER_BIT) |
            translate(bit & BufferBitMask::STENCIL_BUFFER_BIT));
}

void GLDevice::setViewport(int x, int y, int width, int height) {
    // opengl define viewport origin at bottom-left
    glViewport(x, -y, width, height);
}

void GLDevice::setFramebuffer(const Framebuffer *framebuffer) {
    if (framebuffer) {
        framebuffer->bind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GLDevice::setPolygonMode(PolygonMode mode, Face face) {
    glPolygonMode(translate(face), translate(mode));
}

void GLDevice::setDepthMask(bool depthMask) { glDepthMask(depthMask); }

void GLDevice::disable(Operation operation) { glDisable(translate(operation)); }

void GLDevice::enable(Operation operation) { glEnable(translate(operation)); }

void GLDevice::setCullFace(Face cullFace) { glCullFace(translate(cullFace)); }

void GLDevice::setDepthfunc(DepthFunc depthFunc) {
    glDepthFunc(translate(depthFunc));
}

void GLDevice::resetShaderState() { glUseProgram(0); }

}  // namespace sd
