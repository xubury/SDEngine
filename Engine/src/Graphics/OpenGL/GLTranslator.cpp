#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLenum Translate(BufferDataType data_type) {
    switch (data_type) {
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

GLenum Translate(MeshTopology mesh_type) {
    switch (mesh_type) {
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

GLenum Translate(PolygonMode mode) {
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

GLint Translate(BufferIOType io_type) {
    switch (io_type) {
        case BufferIOType::STATIC:
            return GL_STATIC_DRAW;
        case BufferIOType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
    }
    return GL_INVALID_VALUE;
}

GLenum Translate(TextureType texture_type) {
    switch (texture_type) {
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

GLenum TranslateFormat(TextureFormat format, TextureFormatType format_type) {
    switch (format) {
        case TextureFormat::ALPHA:
        case TextureFormat::RED: {
            switch (format_type) {
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RED_INTEGER;
                case TextureFormatType::FLOAT16:
                case TextureFormatType::FLOAT32:
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

GLenum Translate(TextureFormatType format_type) {
    switch (format_type) {
        case TextureFormatType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case TextureFormatType::UINT24_8:
            return GL_UNSIGNED_INT_24_8;
        case TextureFormatType::UINT:
            return GL_UNSIGNED_INT;
        case TextureFormatType::FLOAT16:
        case TextureFormatType::FLOAT32:
            return GL_FLOAT;
    }

    return GL_INVALID_VALUE;
}

GLint TranslateInternalFormat(TextureFormat format,
                              TextureFormatType format_type) {
    switch (format) {
        case TextureFormat::ALPHA:
        case TextureFormat::RED: {
            switch (format_type) {
                case TextureFormatType::UBYTE:
                    return GL_R8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_R32UI;
                case TextureFormatType::FLOAT16:
                    return GL_R16F;
                case TextureFormatType::FLOAT32:
                    return GL_R32F;
            }
            break;
        }
        case TextureFormat::RG: {
            switch (format_type) {
                case TextureFormatType::UBYTE:
                    return GL_RG8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RG32UI;
                case TextureFormatType::FLOAT16:
                    return GL_RG16F;
                case TextureFormatType::FLOAT32:
                    return GL_RG32F;
            }
            break;
        }
        case TextureFormat::RGB: {
            switch (format_type) {
                case TextureFormatType::UBYTE:
                    return GL_RGB8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RGB32UI;
                case TextureFormatType::FLOAT16:
                    return GL_RGB16F;
                case TextureFormatType::FLOAT32:
                    return GL_RGB32F;
            }
            break;
        }
        case TextureFormat::RGBA: {
            switch (format_type) {
                case TextureFormatType::UBYTE:
                    return GL_RGBA8;
                case TextureFormatType::UINT24_8:
                case TextureFormatType::UINT:
                    return GL_RGBA32UI;
                case TextureFormatType::FLOAT16:
                    return GL_RGBA16F;
                case TextureFormatType::FLOAT32:
                    return GL_RGBA32F;
            }
            break;
        }
        case TextureFormat::DEPTH:
            // TODO:This has problem for different video cards
            return GL_DEPTH_COMPONENT24;
        case TextureFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
    }
    return GL_INVALID_VALUE;
}

GLint Translate(TextureWrap wrap) {
    switch (wrap) {
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

GLint Translate(TextureFilter filter) {
    switch (filter) {
        case TextureFilter::NEAREST:
            return GL_NEAREST;
        case TextureFilter::LINEAR:
            return GL_LINEAR;
    }

    return GL_INVALID_VALUE;
}

GLint Translate(TextureMipmapFilter mipmap_filter) {
    switch (mipmap_filter) {
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

GLenum Translate(Operation operation) {
    switch (operation) {
        case Operation::DEPTH_TEST:
            return GL_DEPTH_TEST;
        case Operation::BLEND:
            return GL_BLEND;
        case Operation::STENCIL_TEST:
            return GL_STENCIL_TEST;
        case Operation::MULTISAMPLE:
            return GL_MULTISAMPLE;
        case Operation::CULL_FACE:
            return GL_CULL_FACE;
    }
    return GL_INVALID_ENUM;
}

GLenum Translate(Face face) {
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

GLenum Translate(BufferBitMask bit) {
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

GLenum Translate(DepthFunc depth_func) {
    switch (depth_func) {
        case DepthFunc::EQUAL:
            return GL_EQUAL;
        case DepthFunc::NOT_EQUAL:
            return GL_NOTEQUAL;
        case DepthFunc::GREATER:
            return GL_GREATER;
        case DepthFunc::GREATER_EQUAL:
            return GL_GEQUAL;
        case DepthFunc::LESS:
            return GL_LESS;
        case DepthFunc::LESS_EQUAL:
            return GL_LEQUAL;
    }
    return GL_INVALID_ENUM;
}

}  // namespace SD
