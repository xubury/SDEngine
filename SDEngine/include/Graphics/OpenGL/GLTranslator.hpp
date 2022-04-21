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

GLenum SD_GRAPHICS_API Translate(DataFormat format);

GLenum SD_GRAPHICS_API Translate(TextureType type, MultiSampleLevel msaa);

GLint SD_GRAPHICS_API Translate(TextureWrap wrap);

GLint SD_GRAPHICS_API Translate(TextureMagFilter filter);

GLint SD_GRAPHICS_API Translate(TextureMinFilter filter, MipmapMode mode);

GLenum SD_GRAPHICS_API Translate(Operation operation);

GLenum SD_GRAPHICS_API Translate(Face face);

GLenum SD_GRAPHICS_API Translate(BlitFilter filter);

GLint SD_GRAPHICS_API Translate(BufferBitMask bit);

GLenum SD_GRAPHICS_API Translate(DepthFunc depth_func);

GLenum SD_GRAPHICS_API Translate(Access access);

GLenum SD_GRAPHICS_API Translate(BarrierBit bit);

GLenum SD_GRAPHICS_API GetFormatType(DataFormat format);
GLenum SD_GRAPHICS_API GetDataType(DataFormat format);

}  // namespace SD

#endif /* SD_GL_TRANSLATOR_HPP */
