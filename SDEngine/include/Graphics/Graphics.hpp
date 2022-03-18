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
    Points,
    Lines,
    Triangles,
    Quads,

    Segments,
    Loop
};

enum class PolygonMode { Point, Line, Fill };

enum class MultiSampleLevel { X1 = 1, X2 = 2, X4 = 4, X8 = 8 };

enum class TextureType { Normal, Array, Cube, CubeArray };

enum class DataFormat {
    Alpha8,
    Alpha32I,
    Alpha32UI,
    Alpha16F,
    Alpha32F,

    R8,
    R32I,
    R32UI,
    R16F,
    R32F,

    RG8,
    RG32I,
    RG32UI,
    RG16F,
    RG32F,

    RGB8,
    RGB32I,
    RGB32UI,
    RGB16F,
    RGB32F,

    RGBA8,
    RGBA32I,
    RGBA32UI,
    RGBA16F,
    RGBA32F,

    Depth24,
    Stencil8,
    Depth24Stencil8
};

enum class Face { Front, Back, Both };

enum class BlitFilter { Nearest, Linear };

enum class TextureWrap { Edge, Border, Mirror, Repeat, MirrorEdge };

enum class TextureMagFilter { Nearest, Linear };

enum class TextureMinFilter { Nearest, Linear };

enum class MipmapMode { None, Nearest, Linear };

enum class BufferLayoutType {
    UByte,
    UInt,
    Int,
    Float,
    Float2,
    Float3,
    Float4,
    Mat4
};

enum class BufferIOType { Static, Dynamic };

enum class BufferBitMask {
    None = 0,
    ColorBufferBit = 1,
    DepthBufferBit = 1 << 1,
    StencilBufferBit = 1 << 2
};

template <>
struct enable_bitmask_operators<BufferBitMask> {
    static constexpr bool enable = true;
};

enum class Operation { DepthTest, Blend, StencilTest, MSAA, FaceCulling };

enum class DepthFunc {
    Less,
    LessEqual,
    Equal,
    Greater,
    GreaterEqual,
    NotEqual
};

}  // namespace SD

#endif /* SD_GRAPHICS_HPP */
