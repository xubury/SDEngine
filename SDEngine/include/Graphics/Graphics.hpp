#ifndef SD_GRAPHICS_HPP
#define SD_GRAPHICS_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"

#include <type_traits>
#include <array>

namespace SD {

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

enum class DataFormat { ALPHA, RED, RG, RGB, RGBA, DEPTH, DEPTH_STENCIL };

enum class DataFormatType { UBYTE, UINT, INT, FLOAT16, FLOAT32 };

enum class Face { FRONT, BACK, BOTH };

enum class BlitFilter { NEAREST, LINEAR };

enum class TextureWrap { EDGE, BORDER, MIRROR, REPEAT, MIRROR_EDGE };

enum class TextureMagFilter { NEAREST, LINEAR };

enum class TextureMinFilter {
    LINEAR,
    NEAREST,
    NEAREST_NEAREST,
    NEAREST_LINEAR,
    LINEAR_NEAREST,
    LINEAR_LINEAR,
};

enum class BufferLayoutType { UBYTE, UINT, INT, FLOAT, FLOAT2, FLOAT3, FLOAT4 };

enum class BufferIOType { STATIC, DYNAMIC };

enum class BufferBitMask {
    COLOR_BUFFER_BIT = 1,
    DEPTH_BUFFER_BIT = 1 << 1,
    STENCIL_BUFFER_BIT = 1 << 2
};

template <>
struct enable_bitmask_operators<BufferBitMask> {
    static constexpr bool enable = true;
};

enum class Operation {
    DEPTH_TEST,
    BLEND,
    STENCIL_TEST,
    MULTISAMPLE,
    CULL_FACE
};

enum class DepthFunc {
    LESS,
    LESS_EQUAL,
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    NOT_EQUAL
};

enum class CubeMapFace {
    RIGHT = 0,
    LEFT,
    TOP,
    BOTTOM,
    FRONT,
    BACK,

    NUMS
};

}  // namespace SD

#endif /* SD_GRAPHICS_HPP */
