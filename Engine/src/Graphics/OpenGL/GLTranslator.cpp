#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Assert.hpp"

namespace sd {

GLenum translate(BufferDataType dataType) {
    switch (dataType) {
        case BufferDataType::FLOAT:
        case BufferDataType::FLOAT2:
        case BufferDataType::FLOAT3:
        case BufferDataType::FLOAT4:
            return GL_FLOAT;
        case BufferDataType::UINT:
            return GL_UNSIGNED_INT;
        case BufferDataType::UCHAR:
            return GL_UNSIGNED_BYTE;
    }
    return GL_INVALID_ENUM;
}

GLenum translate(MeshTopology meshType) {
    switch (meshType) {
        case MeshTopology::TRIANGLES:
            return GL_TRIANGLES;
        case MeshTopology::LINES:
            return GL_LINES;
        case MeshTopology::POINTS:
            return GL_POINTS;
        case MeshTopology::LOOP:
            return GL_LINE_LOOP;
        case MeshTopology::QUADS:
            return GL_QUADS;
        case MeshTopology::SEGMENTS:
            return GL_LINE_STRIP;
    }
    return GL_INVALID_ENUM;
}

GLenum translate(PolygonMode mode) {
    switch (mode) {
        case PolygonMode::POINT:
            return GL_POINT;
        case PolygonMode::LINE:
            return GL_LINE;
        case PolygonMode::FILL:
            return GL_FILL;
    }
    return GL_INVALID_VALUE;
}

GLint translate(BufferIOType ioType) {
    switch (ioType) {
        case BufferIOType::STATIC:
            return GL_STATIC_DRAW;
        case BufferIOType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
    }
    return GL_INVALID_VALUE;
}

GLenum translate(TextureType textureType) {
    switch (textureType) {
        case TextureType::TEX_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEX_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case TextureType::TEX_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEX_CUBE:
            return GL_TEXTURE_CUBE_MAP;
    }

    return GL_INVALID_ENUM;
}

GLenum translateFormat(TextureFormat textureFormat,
                       TextureFormatType textureFormatType) {
    switch (textureFormat) {
        case TextureFormat::ALPHA:
        case TextureFormat::RED: {
            switch (textureFormatType) {
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RED_INTEGER;
                case TextureFormatType::FLOAT:
                case TextureFormatType::UBYTE:
                    return GL_RED;
            }
        } break;
        case TextureFormat::RG:
            return GL_RG;
        case TextureFormat::RGB:
            return GL_RGB;
        case TextureFormat::RGBA:
            return GL_RGBA;
        case TextureFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
    }
    return GL_INVALID_ENUM;
}

GLenum translate(TextureFormatType textureFormatType) {
    switch (textureFormatType) {
        case TextureFormatType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case TextureFormatType::UINT24_8:
            return GL_UNSIGNED_INT_24_8;
        case TextureFormatType::UINT:
            return GL_UNSIGNED_INT;
        case TextureFormatType::FLOAT:
            return GL_FLOAT;
    }

    return GL_INVALID_VALUE;
}

GLint translateInternalFormat(TextureFormat textureFormat,
                              TextureFormatType textureFormatType) {
    switch (textureFormat) {
        case TextureFormat::ALPHA:
        case TextureFormat::RED: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_R8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_R32UI;
                case TextureFormatType::FLOAT:
                    return GL_R32F;
            }
            break;
        }
        case TextureFormat::RG: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RG8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RG32UI;
                case TextureFormatType::FLOAT:
                    return GL_RG32F;
            }
            break;
        }
        case TextureFormat::RGB: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RGB8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RGB32UI;
                case TextureFormatType::FLOAT:
                    return GL_RGB16F;
            }
            break;
        }
        case TextureFormat::RGBA: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RGBA8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RGBA32UI;
                case TextureFormatType::FLOAT:
                    return GL_RGB16F;
            }
            break;
        }
        case TextureFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
    }
    return GL_INVALID_VALUE;
}

GLint translate(TextureWrap textureWrap) {
    switch (textureWrap) {
        case TextureWrap::EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::BORDER:
            return GL_CLAMP_TO_BORDER;
        case TextureWrap::MIRROR:
            return GL_MIRRORED_REPEAT;
        case TextureWrap::REPEAT:
            return GL_REPEAT;
        case TextureWrap::MIRROR_EDGE:
            return GL_MIRROR_CLAMP_TO_EDGE;
    }

    return GL_INVALID_VALUE;
}

GLint translate(TextureFilter textureFilter) {
    switch (textureFilter) {
        case TextureFilter::NEAREST:
            return GL_NEAREST;
        case TextureFilter::LINEAR:
            return GL_LINEAR;
    }

    return GL_INVALID_VALUE;
}

GLint translate(TextureMipmapFilter textureMipmapFilter) {
    switch (textureMipmapFilter) {
        case TextureMipmapFilter::LINEAR:
            return GL_LINEAR;
        case TextureMipmapFilter::NEAREST:
            return GL_NEAREST;
        case TextureMipmapFilter::NEAREST_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case TextureMipmapFilter::NEAREST_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case TextureMipmapFilter::LINEAR_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case TextureMipmapFilter::LINEAR_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_INVALID_VALUE;
}

GLenum translate(Operation operation) {
    switch (operation) {
        case Operation::DEPTH_TEST:
            return GL_DEPTH_TEST;
        case Operation::BLEND:
            return GL_BLEND;
        case Operation::STENCIL_TEST:
            return GL_STENCIL_TEST;
        case Operation::MULTISAMPLE:
            return GL_MULTISAMPLE;
    }
    return GL_INVALID_ENUM;
}

GLenum translate(Face face) {
    switch (face) {
        case Face::FRONT:
            return GL_FRONT;
        case Face::BACK:
            return GL_BACK;
        case Face::BOTH:
            return GL_FRONT_AND_BACK;
    }
    return GL_INVALID_ENUM;
}

GLenum translate(BufferBitMask bit) {
    switch (bit) {
        case BufferBitMask::COLOR_BUFFER_BIT:
            return GL_COLOR_BUFFER_BIT;
        case BufferBitMask::DEPTH_BUFFER_BIT:
            return GL_DEPTH_BUFFER_BIT;
        case BufferBitMask::STENCIL_BUFFER_BIT:
            return GL_STENCIL_BUFFER_BIT;
    }
    return 0;
}

}  // namespace sd
