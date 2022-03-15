#include "Graphics/OpenGL/GLRenderbuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

#include <GL/glew.h>

namespace SD {

GLRenderbuffer::GLRenderbuffer(int width, int height, MultiSampleLevel samples,
                               DataFormat format)
    : Renderbuffer(width, height, samples, format) {
    Allocate();
}

void GLRenderbuffer::Allocate() {
    glCreateRenderbuffers(1, &m_id);

    GLint gl_internal_format = Translate(m_format);
    GLsizei samples = static_cast<GLsizei>(m_samples);
    if (samples > 1) {
        glNamedRenderbufferStorageMultisample(m_id, samples, gl_internal_format,
                                              m_width, m_height);
    } else {
        glNamedRenderbufferStorage(m_id, gl_internal_format, m_width, m_height);
    }
}
}  // namespace SD
