#include "Graphics/OpenGL/GLRenderbuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLRenderbuffer::GLRenderbuffer(int width, int height, int samples,
                               DataFormat format, DataFormatType format_type)
    : Renderbuffer(width, height, samples, format, format_type) {
    Allocate();
}

void GLRenderbuffer::Allocate() {
    glCreateRenderbuffers(1, &m_id);

    GLint gl_internal_format = TranslateInternalFormat(m_format, m_format_type);
    if (m_samples > 1) {
        glNamedRenderbufferStorageMultisample(
            m_id, m_samples, gl_internal_format, m_width, m_height);
    } else {
        glNamedRenderbufferStorage(m_id, gl_internal_format, m_width,
                                   m_height);
    }
}
}  // namespace SD
