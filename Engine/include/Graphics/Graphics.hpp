#ifndef SD_GRAPHICS_HPP
#define SD_GRAPHICS_HPP

#include "Utility/Loader/AssetLoader.hpp"

namespace sd {

template <typename E>
struct enable_bitmask_operators {
    static constexpr bool enable = false;
};

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator|(
    E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(lhs) |
                          static_cast<underlying>(rhs));
}

template <typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type operator&(
    E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(lhs) &
                          static_cast<underlying>(rhs));
}

enum class MeshTopology {
    POINTS,
    LINES,
    TRIANGLES,
    QUADS,

    SEGMENTS,
    LOOP
};

enum class PolygonMode { POINT, LINE, FILL };

enum class TextureType { TEX_2D, TEX_2D_MULTISAMPLE, TEX_3D, TEX_CUBE };

enum class TextureFormatType { UBYTE, UINT, FLOAT };

enum class Face { FRONT, BACK, BOTH };

enum class TextureFormat { ALPHA, RED, RG, RGB, RGBA, DEPTH, STENCIL };

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

enum class BufferBitMask : uint32_t {
    COLOR_BUFFER_BIT = 1,
    DEPTH_BUFFER_BIT = 1 << 1,
    STENCIL_BUFFER_BIT = 1 << 2
};

template <>
struct enable_bitmask_operators<BufferBitMask> {
    static constexpr bool enable = true;
};

enum class Operation { DEPTH_TEST, BLEND, STENCIL_TEST, MULTISAMPLE };

class SD_API Graphics {
   public:
    static void init(API api);

    static API getAPI();
};

}  // namespace sd

#endif /* SD_GRAPHICS_MANAGER_HPP */
