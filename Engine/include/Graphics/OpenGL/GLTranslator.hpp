#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include <GL/glew.h>
#include "Graphics/Graphics.hpp"

namespace sd {

GLenum SD_API translate(BufferDataType dataType);

GLenum SD_API translate(MeshTopology meshType);

GLint SD_API translate(BufferIOType ioType);

GLenum SD_API translate(TextureType textureType);

GLenum SD_API translateFormat(TextureFormat textureFormat,
                              TextureFormatType textureFormatType);

GLenum SD_API translate(TextureFormatType textureFormatType);

GLint SD_API translateInternalFormat(TextureFormat textureFormat,
                                     TextureFormatType textureFormatType);

GLint SD_API translate(TextureWrap textureWrap);

GLint SD_API translate(TextureFilter textureFilter);

GLint SD_API translate(TextureMipmapFilter textureMipmapFilter);

GLenum SD_API translate(Operation operation);

}  // namespace sd

#endif /* SD_GL_TRANSLATOR_HPP */
