#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include "Graphics/Translator.hpp"
#include <GL/glew.h>

namespace sd {

class GLTranslator : public Translator<GLint, GLboolean, GLenum> {
   public:
    static GLTranslator &instance();

    GLboolean translate(Boolean boolean) override;

    GLenum translate(DataType dataType) override;

    GLenum translate(MeshTopology meshType) override;

    GLint translate(BufferType bufferType) override;

    GLint translate(BufferIOType ioType) override;
};

#ifdef TRANSLATE
#error "Translaor Conflict!"
#else
#define TRANSLATE GLTranslator::instance().translate
#endif

}  // namespace sd

#endif
