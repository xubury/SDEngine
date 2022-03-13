#include "Graphics/OpenGL/GLRenderbuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

#include <GL/glew.h>

namespace SD {

GLRenderbuffer::GLRenderbuffer(int width, int height, int samples,
                               DataFormat format)
    : Renderbuffer(width, height, samples, format) {
    Allocate();
}

void GLRenderbuffer::Allocate() {
    glCreateRenderbuffers(1, &m_id);

    GLint gl_internal_format = Translate(m_format);
    if (m_samples > 1) {
        glNamedRenderbufferStorageMultisample(
            m_id, m_samples, gl_internal_format, m_width, m_height);
    } else {
        glNamedRenderbufferStorage(m_id, gl_internal_format, m_width, m_height);
    }
}
}  // namespace SD
