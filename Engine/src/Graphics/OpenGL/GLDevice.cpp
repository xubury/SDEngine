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
            SD_CORE_CRITICAL(message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
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

void GLDevice::init() {
#ifdef DEBUG_BUILD
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    // Depth Test
    setDepthTest(true);

    // Blend
    setBlend(true);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLDevice::drawElements(MeshTopology topology, size_t count,
                            size_t offset) {
    glDrawElements(translate(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GLDevice::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

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

void GLDevice::setWireframe(bool wireframe) {
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void GLDevice::setDepthMask(bool depthMask) { glDepthMask(depthMask); }

void GLDevice::setBlend(bool blend) {
    blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void GLDevice::setDepthTest(bool depthTest) {
    depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void GLDevice::setMultisample(bool multisample) {
    multisample ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
}

void GLDevice::setCullFace(CullFace cullFace) {
    switch (cullFace) {
        case CullFace::FRONT:
            glCullFace(GL_FRONT);
            break;
        case CullFace::BACK:
            glCullFace(GL_BACK);
            break;
        case CullFace::BOTH:
            glCullFace(GL_FRONT_AND_BACK);
            break;
    }
}

}  // namespace sd
