#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include <GL/glew.h>
#include "Utility/Export.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

GLenum SD_API Translate(BufferDataType data_type);

GLenum SD_API Translate(MeshTopology mesh_type);

GLenum SD_API Translate(PolygonMode mode);

GLint SD_API Translate(BufferIOType io_type);

GLenum SD_API Translate(TextureType texture_type);

GLenum SD_API TranslateFormat(TextureFormat format,
                              TextureFormatType format_type);

GLenum SD_API Translate(TextureFormatType format_type);

GLint SD_API TranslateInternalFormat(TextureFormat format,
                                     TextureFormatType format_type);

GLint SD_API Translate(TextureWrap wrap);

GLint SD_API Translate(TextureFilter filter);

GLint SD_API Translate(TextureMipmapFilter mipmap_filter);

GLenum SD_API Translate(Operation operation);

GLenum SD_API Translate(Face face);

GLenum SD_API Translate(BufferBitMask bit);

GLenum SD_API Translate(DepthFunc depth_func);

}  // namespace SD

#endif /* SD_GL_TRANSLATOR_HPP */
