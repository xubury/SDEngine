#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include "Graphics/Translator.hpp"
#include <GL/glew.h>

namespace sd {

class GLTranslator : public Translator<GLint, GLboolean, GLenum> {
   public:
    static GLTranslator &instance();

    GLenum translate(BufferDataType dataType) override;

    GLenum translate(MeshTopology meshType) override;

    GLint translate(BufferIOType ioType) override;

    GLenum translate(TextureType textureType) override;

    GLenum translate(TextureFormat textureFormat) override;

    GLenum translate(TextureFormatType textureFormatType) override;

    GLint translate(TextureFormat textureFormat,
                    TextureFormatType textureFormatType) override;

    GLint translate(TextureWrap textureWrap) override;

    GLint translate(TextureFilter textureFilter) override;

    GLint translate(TextureMipmapFilter textureMipmapFilter) override;
};

#ifdef TRANSLATE
#error "Translaor Conflict!"
#else
#define TRANSLATE GLTranslator::instance().translate
#endif

}  // namespace sd

#endif /* SD_GL_TRANSLATOR_HPP */
