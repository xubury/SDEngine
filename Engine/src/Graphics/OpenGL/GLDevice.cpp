#include "Graphics/OpenGL/GLDevice.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include <GL/glew.h>

namespace sd {

void GLDevice::drawElements(MeshTopology topology, size_t count,
                            size_t offset) {
    glDrawElements(TRANSLATE(topology), count, GL_UNSIGNED_INT,
                   (const void *)offset);
}

void GLDevice::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void GLDevice::clear() { glClear(GL_COLOR_BUFFER_BIT); }

void GLDevice::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

}  // namespace sd
