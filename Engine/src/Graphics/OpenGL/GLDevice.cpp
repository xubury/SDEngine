#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Graphics/OpenGL/GLFramebuffer.hpp"
#include <GL/glew.h>

namespace sd {

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

}  // namespace sd
