#ifndef SD_GL_TRANSLATOR_HPP
#define SD_GL_TRANSLATOR_HPP

#include <GL/glew.h>
#include "Utility/Export.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

GLenum SD_GRAPHICS_API Translate(BufferLayoutType data_type);

GLenum SD_GRAPHICS_API Translate(MeshTopology mesh_type);

GLenum SD_GRAPHICS_API Translate(PolygonMode mode);

GLint SD_GRAPHICS_API Translate(BufferIOType io_type);

GLenum SD_GRAPHICS_API Translate(DataFormatType format_type);

GLenum SD_GRAPHICS_API Translate(TextureType texture_type);

GLenum SD_GRAPHICS_API TranslateFormat(DataFormat format,
                                       DataFormatType format_type);

GLenum SD_GRAPHICS_API TranslateInternalFormat(DataFormat format,
                                               DataFormatType format_type);

GLint SD_GRAPHICS_API Translate(TextureWrap wrap);

GLint SD_GRAPHICS_API Translate(TextureMagFilter filter);

GLint SD_GRAPHICS_API Translate(TextureMinFilter min_filter);

GLenum SD_GRAPHICS_API Translate(Operation operation);

GLenum SD_GRAPHICS_API Translate(Face face);

GLenum SD_GRAPHICS_API Translate(BlitFilter filter);

GLint SD_GRAPHICS_API Translate(BufferBitMask bit);

GLenum SD_GRAPHICS_API Translate(DepthFunc depth_func);

}  // namespace SD

#endif /* SD_GL_TRANSLATOR_HPP */
