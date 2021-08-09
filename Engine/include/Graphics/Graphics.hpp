#ifndef SD_GRAPHICS_HPP
#define SD_GRAPHICS_HPP

#include "Utils/AssetManager.hpp"

namespace sd {

enum class MeshTopology {
    POINTS,
    LINES,
    TRIANGLES,
    QUADS,

    SEGMENTS,
    LOOP
};

enum class TextureType { TEX_2D, TEX_2D_MULTISAMPLE, TEX_3D, TEX_CUBE };

enum class TextureFormatType { UBYTE, FLOAT };

enum class CullFace { FRONT, BACK, BOTH };

// GL has types for every type of value in diffrent bit depths :c but they
// also dont work so...
enum class TextureFormat { ALPHA, RG, RGB, RGBA, DEPTH, STENCIL };

enum class TextureWrap { EDGE, BORDER, MIRROR, REPEAT, MIRROR_EDGE };

enum class TextureFilter { NEAREST, LINEAR };

enum class TextureMipmapFilter {
    LINEAR,
    NEAREST,
    NEAREST_NEAREST,
    NEAREST_LINEAR,
    LINEAR_NEAREST,
    LINEAR_LINEAR,
};

enum class BufferDataType { FLOAT, UINT, UCHAR };

enum class BufferIOType { STATIC, DYNAMIC };

enum class API { None, OpenGL };

enum BufferBit {
    COLOR_BUFFER_BIT = 1,
    DEPTH_BUFFER_BIT = 1 << 1,
    STENCIL_BUFFER_BIT = 1 << 2
};

class Graphics {
   public:
    static void init(API api);

    static API getAPI();

    static AssetManager &assetManager();
};

}  // namespace sd

#endif /* SD_GRAPHICS_MANAGER_HPP */
