#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include <GL/glew.h>
#include "Utility/Export.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

GLenum SD_API Translate(BufferDataType dataType);

GLenum SD_API Translate(MeshTopology meshType);

GLenum SD_API Translate(PolygonMode mode);

GLint SD_API Translate(BufferIOType ioType);

GLenum SD_API Translate(TextureType textureType);

GLenum SD_API TranslateFormat(TextureFormat textureFormat,
                              TextureFormatType textureFormatType);

GLenum SD_API Translate(TextureFormatType textureFormatType);

GLint SD_API TranslateInternalFormat(TextureFormat textureFormat,
                                     TextureFormatType textureFormatType);

GLint SD_API Translate(TextureWrap textureWrap);

GLint SD_API Translate(TextureFilter textureFilter);

GLint SD_API Translate(TextureMipmapFilter textureMipmapFilter);

GLenum SD_API Translate(Operation operation);

GLenum SD_API Translate(Face face);

GLenum SD_API Translate(BufferBitMask bit);

GLenum SD_API Translate(DepthFunc depthFunc);

}  // namespace SD

#endif /* SD_GL_TRANSLATOR_HPP */
