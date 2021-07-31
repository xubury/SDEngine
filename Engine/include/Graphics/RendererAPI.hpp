#ifndef SD_RENDERER_API_HPP
#define SD_RENDERER_API_HPP

namespace sd {

#include <stdint.h>

enum class MeshTopology {
    POINTS,
    LINES,
    TRIANGLES,
    QUADS,

    SEGMENTS,
    LOOP
};

enum class TextureType { TEX_2D, TEX_3D, TEX_CUBE };

enum TextureFormatType { UBYTE, FLOAT };

enum class CullFace { FRONT, BACK, BOTH };

// GL has types for every type of value in diffrent bit depths :c but they
// also dont work so...
enum class TextureFormat { ALPHA, RG, RGB, RGBA, DEPTH, STENCIL };

enum class TextureWrap { EDGE, BORDER, MIRROR, REPEAT, MIRROR_EDGE };

enum class TextureFilter { NEAREST, LINEAR };

enum class TextureMipmapFilter {
    NEAREST_NEAREST,
    NEAREST_LINEAR,
    LINEAR_NEAREST,
    LINEAR_LINEAR,
};

enum class DataType { FLOAT, UINT, UCHAR };

enum class Boolean { FALSE, TRUE };

enum class BufferType { VERTEX, INDEX, UNIFORM };

enum class BufferIOType { STATIC, DYNAMIC };

enum class API { None, OpenGL };

class RendererAPI {
   public:
    static void drawElements(MeshTopology topology, size_t count,
                             size_t offset);

    static void setClearColor(float r, float g, float b, float a);

    static void clear();

    static void setViewport(int x, int y, int width, int height);

};  // namespace RendererAPI

}  // namespace sd

#endif /* SD_RENDERER_API_HPP */
