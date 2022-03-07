#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLenum Translate(BufferLayoutType data_type) {
    switch (data_type) {
        case BufferLayoutType::FLOAT:
        case BufferLayoutType::FLOAT2:
        case BufferLayoutType::FLOAT3:
        case BufferLayoutType::FLOAT4:
            return GL_FLOAT;
        case BufferLayoutType::UINT:
            return GL_UNSIGNED_INT;
        case BufferLayoutType::INT:
            return GL_INT;
        case BufferLayoutType::UBYTE:
            return GL_UNSIGNED_BYTE;
    }
    return 0;
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
    return 0;
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
    return 0;
}

GLint Translate(BufferIOType io_type) {
    switch (io_type) {
        case BufferIOType::STATIC:
            return GL_STATIC_DRAW;
        case BufferIOType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
    }
    return 0;
}

GLenum Translate(TextureType texture_type) {
    switch (texture_type) {
        case TextureType::TEX_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEX_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case TextureType::TEX_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;
        case TextureType::TEX_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEX_CUBE:
            return GL_TEXTURE_CUBE_MAP;
    }

    return 0;
}

GLenum TranslateFormat(DataFormat format, DataFormatType format_type) {
    switch (format) {
        case DataFormat::ALPHA:
        case DataFormat::RED: {
            switch (format_type) {
                case DataFormatType::INT:
                case DataFormatType::UINT:
                    return GL_RED_INTEGER;
                default:
                    return GL_RED;
            }
        } break;
        case DataFormat::RG: {
            switch (format_type) {
                case DataFormatType::INT:
                case DataFormatType::UINT:
                    return GL_RG_INTEGER;
                default:
                    return GL_RG;
            }
        } break;
        case DataFormat::RGB: {
            switch (format_type) {
                case DataFormatType::INT:
                case DataFormatType::UINT:
                    return GL_RGB_INTEGER;
                default:
                    return GL_RGB;
            }
        } break;
        case DataFormat::RGBA: {
            switch (format_type) {
                case DataFormatType::INT:
                case DataFormatType::UINT:
                    return GL_RGBA_INTEGER;
                default:
                    return GL_RGBA;
            }
        } break;
        case DataFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        case DataFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
    }
    return 0;
}

GLenum Translate(DataFormatType format_type) {
    switch (format_type) {
        case DataFormatType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case DataFormatType::UINT:
            return GL_UNSIGNED_INT;
        case DataFormatType::INT:
            return GL_INT;
        case DataFormatType::FLOAT16:
        case DataFormatType::FLOAT32:
            return GL_FLOAT;
    }

    return 0;
}

GLenum TranslateInternalFormat(DataFormat format, DataFormatType format_type) {
    switch (format) {
        case DataFormat::ALPHA:
        case DataFormat::RED: {
            switch (format_type) {
                case DataFormatType::UBYTE:
                    return GL_R8;
                case DataFormatType::UINT:
                    return GL_R32UI;
                case DataFormatType::INT:
                    return GL_R32I;
                case DataFormatType::FLOAT16:
                    return GL_R16F;
                case DataFormatType::FLOAT32:
                    return GL_R32F;
            }
            break;
        }
        case DataFormat::RG: {
            switch (format_type) {
                case DataFormatType::UBYTE:
                    return GL_RG8;
                case DataFormatType::UINT:
                    return GL_RG32UI;
                case DataFormatType::INT:
                    return GL_RG32I;
                case DataFormatType::FLOAT16:
                    return GL_RG16F;
                case DataFormatType::FLOAT32:
                    return GL_RG32F;
            }
            break;
        }
        case DataFormat::RGB: {
            switch (format_type) {
                case DataFormatType::UBYTE:
                    return GL_RGB8;
                case DataFormatType::UINT:
                    return GL_RGB32UI;
                case DataFormatType::INT:
                    return GL_RGB32I;
                case DataFormatType::FLOAT16:
                    return GL_RGB16F;
                case DataFormatType::FLOAT32:
                    return GL_RGB32F;
            }
            break;
        }
        case DataFormat::RGBA: {
            switch (format_type) {
                case DataFormatType::UBYTE:
                    return GL_RGBA8;
                case DataFormatType::UINT:
                    return GL_RGBA32UI;
                case DataFormatType::INT:
                    return GL_RGBA32I;
                case DataFormatType::FLOAT16:
                    return GL_RGBA16F;
                case DataFormatType::FLOAT32:
                    return GL_RGBA32F;
            }
            break;
        }
        case DataFormat::DEPTH:
            // TODO:This has problem for different video cards
            return GL_DEPTH_COMPONENT24;
        case DataFormat::DEPTH_STENCIL:
            return GL_DEPTH24_STENCIL8;
    }
    return 0;
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

    return 0;
}

GLint Translate(TextureMagFilter filter) {
    switch (filter) {
        case TextureMagFilter::NEAREST:
            return GL_NEAREST;
        case TextureMagFilter::LINEAR:
            return GL_LINEAR;
    }

    return 0;
}

GLint Translate(TextureMinFilter min_filter) {
    switch (min_filter) {
        case TextureMinFilter::LINEAR:
            return GL_LINEAR;
        case TextureMinFilter::NEAREST:
            return GL_NEAREST;
        case TextureMinFilter::NEAREST_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case TextureMinFilter::NEAREST_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case TextureMinFilter::LINEAR_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case TextureMinFilter::LINEAR_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    return 0;
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
    return 0;
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
    return 0;
}

GLenum Translate(BlitFilter filter) {
    switch (filter) {
        case BlitFilter::NEAREST:
            return GL_NEAREST;
        case BlitFilter::LINEAR:
            return GL_LINEAR;
    }
    return 0;
}

GLint Translate(BufferBitMask bit) {
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
    return 0;
}

}  // namespace SD
