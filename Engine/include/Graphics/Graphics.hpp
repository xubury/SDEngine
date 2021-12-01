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

enum class TextureFormatType { UBYTE, UINT, FLOAT, UINT24_8 };

enum class Face { FRONT, BACK, BOTH };

enum class TextureFormat { ALPHA, RED, RG, RGB, RGBA, DEPTH, DEPTH_STENCIL };

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

enum class BufferDataType { UCHAR, UINT, FLOAT, FLOAT2, FLOAT3, FLOAT4 };

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

inline constexpr uint8_t GetCubeMapFaceMask(CubeMapFace face) {
    return 1 << static_cast<uint8_t>(face);
}

inline CubeMapFace operator++(CubeMapFace &face) {
    face = static_cast<CubeMapFace>(static_cast<int>(face) + 1);
    return face;
}

const std::string SD_GRAPHICS_API GetFaceName(CubeMapFace face);

enum CubeMapFaceMask {
    CUBE_MAP_FACE_MASK_RIGHT = GetCubeMapFaceMask(CubeMapFace::RIGHT),
    CUBE_MAP_FACE_MASK_LEFT = GetCubeMapFaceMask(CubeMapFace::LEFT),
    CUBE_MAP_FACE_MASK_TOP = GetCubeMapFaceMask(CubeMapFace::TOP),
    CUBE_MAP_FACE_MASK_BOTTOM = GetCubeMapFaceMask(CubeMapFace::BOTTOM),
    CUBE_MAP_FACE_MASK_FRONT = GetCubeMapFaceMask(CubeMapFace::FRONT),
    CUBE_MAP_FACE_MASK_BACK = GetCubeMapFaceMask(CubeMapFace::BACK),
    CUBE_MAP_FACE_ALL = GetCubeMapFaceMask(CubeMapFace::NUMS) - 1,
};

struct SD_GRAPHICS_API RGBA {
   public:
    RGBA(float r, float g, float b, float a) : m_color{r, g, b, a} {}

    float Red() { return m_color[0]; }
    float Green() { return m_color[1]; }
    float Blue() { return m_color[2]; }
    float Alpha() { return m_color[3]; }

    float *Data() { return m_color.data(); }
    const float *Data() const { return m_color.data(); }

   private:
    std::array<float, 4> m_color;
};

enum class GraphicsAPI { None, OpenGL };

void SD_GRAPHICS_API SetGraphicsAPI(GraphicsAPI api);

GraphicsAPI SD_GRAPHICS_API GetGraphicsAPI();

}  // namespace SD

#endif /* SD_GRAPHICS_HPP */
