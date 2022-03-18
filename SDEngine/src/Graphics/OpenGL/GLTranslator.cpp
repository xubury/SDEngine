#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLenum Translate(BufferLayoutType data_type) {
    switch (data_type) {
        case BufferLayoutType::Float:
        case BufferLayoutType::Float2:
        case BufferLayoutType::Float3:
        case BufferLayoutType::Float4:
        case BufferLayoutType::Mat4:
            return GL_FLOAT;
        case BufferLayoutType::UInt:
            return GL_UNSIGNED_INT;
        case BufferLayoutType::Int:
            return GL_INT;
        case BufferLayoutType::UByte:
            return GL_UNSIGNED_BYTE;
    }
    return 0;
}

GLenum Translate(MeshTopology mesh_type) {
    switch (mesh_type) {
        case MeshTopology::Triangles:
            return GL_TRIANGLES;
        case MeshTopology::Lines:
            return GL_LINES;
        case MeshTopology::Points:
            return GL_POINTS;
        case MeshTopology::Loop:
            return GL_LINE_LOOP;
        case MeshTopology::Quads:
            return GL_QUADS;
        case MeshTopology::Segments:
            return GL_LINE_STRIP;
    }
    return 0;
}

GLenum Translate(PolygonMode mode) {
    switch (mode) {
        case PolygonMode::Point:
            return GL_POINT;
        case PolygonMode::Line:
            return GL_LINE;
        case PolygonMode::Fill:
            return GL_FILL;
    }
    return 0;
}

GLint Translate(BufferIOType io_type) {
    switch (io_type) {
        case BufferIOType::Static:
            return GL_STATIC_DRAW;
        case BufferIOType::Dynamic:
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

GLenum Translate(DataFormat format) {
    switch (format) {
        case DataFormat::Alpha8:
        case DataFormat::R8:
            return GL_R8;
        case DataFormat::RG8:
            return GL_RG8;
        case DataFormat::RGB8:
            return GL_RGB8;
        case DataFormat::RGBA8:
            return GL_RGBA8;
        case DataFormat::Alpha32I:
        case DataFormat::R32I:
            return GL_R32I;
        case DataFormat::RG32I:
            return GL_RG32I;
        case DataFormat::RGB32I:
            return GL_RGB32I;
        case DataFormat::RGBA32I:
            return GL_RGBA32I;
        case DataFormat::Alpha32UI:
        case DataFormat::R32UI:
            return GL_R32UI;
        case DataFormat::RG32UI:
            return GL_RG32UI;
        case DataFormat::RGB32UI:
            return GL_RGB32UI;
        case DataFormat::RGBA32UI:
            return GL_RGBA32UI;
        case DataFormat::Alpha16F:
        case DataFormat::R16F:
            return GL_R16F;
        case DataFormat::RG16F:
            return GL_RG16F;
        case DataFormat::RGB16F:
            return GL_RGB16F;
        case DataFormat::RGBA16F:
            return GL_RGBA16F;
        case DataFormat::Alpha32F:
        case DataFormat::R32F:
            return GL_R32F;
        case DataFormat::RG32F:
            return GL_RG32F;
        case DataFormat::RGB32F:
            return GL_RGB32F;
        case DataFormat::RGBA32F:
            return GL_RGBA32F;
        case DataFormat::Depth24:
            return GL_DEPTH_COMPONENT24;
        case DataFormat::Stencil8:
            return GL_STENCIL_INDEX8;
        case DataFormat::Depth24Stencil8:
            return GL_DEPTH24_STENCIL8;
    }
    return 0;
}

GLint Translate(TextureWrap wrap) {
    switch (wrap) {
        case TextureWrap::Edge:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::Border:
            return GL_CLAMP_TO_BORDER;
        case TextureWrap::Mirror:
            return GL_MIRRORED_REPEAT;
        case TextureWrap::Repeat:
            return GL_REPEAT;
        case TextureWrap::MirrorEdge:
            return GL_MIRROR_CLAMP_TO_EDGE;
    }

    return 0;
}

GLint Translate(TextureMagFilter filter) {
    switch (filter) {
        case TextureMagFilter::Nearest:
            return GL_NEAREST;
        case TextureMagFilter::Linear:
            return GL_LINEAR;
    }

    return 0;
}

GLint Translate(TextureMinFilter filter, MipmapMode mode) {
    switch (filter) {
        case TextureMinFilter::Linear:
            switch (mode) {
                case MipmapMode::None:
                    return GL_LINEAR;
                case MipmapMode::Linear:
                    return GL_LINEAR_MIPMAP_LINEAR;
                case MipmapMode::Nearest:
                    return GL_LINEAR_MIPMAP_NEAREST;
            }
        case TextureMinFilter::Nearest:
            switch (mode) {
                case MipmapMode::None:
                    return GL_NEAREST;
                case MipmapMode::Linear:
                    return GL_NEAREST_MIPMAP_LINEAR;
                case MipmapMode::Nearest:
                    return GL_NEAREST_MIPMAP_NEAREST;
            }
    }
    return 0;
}

GLenum Translate(Operation operation) {
    switch (operation) {
        case Operation::DepthTest:
            return GL_DEPTH_TEST;
        case Operation::Blend:
            return GL_BLEND;
        case Operation::StencilTest:
            return GL_STENCIL_TEST;
        case Operation::MSAA:
            return GL_MULTISAMPLE;
        case Operation::FaceCulling:
            return GL_CULL_FACE;
    }
    return 0;
}

GLenum Translate(Face face) {
    switch (face) {
        case Face::Front:
            return GL_FRONT;
        case Face::Back:
            return GL_BACK;
        case Face::Both:
            return GL_FRONT_AND_BACK;
    }
    return 0;
}

GLenum Translate(BlitFilter filter) {
    switch (filter) {
        case BlitFilter::Nearest:
            return GL_NEAREST;
        case BlitFilter::Linear:
            return GL_LINEAR;
    }
    return 0;
}

GLint Translate(BufferBitMask bit) {
    switch (bit) {
        case BufferBitMask::None:
            return 0;
        case BufferBitMask::ColorBufferBit:
            return GL_COLOR_BUFFER_BIT;
        case BufferBitMask::DepthBufferBit:
            return GL_DEPTH_BUFFER_BIT;
        case BufferBitMask::StencilBufferBit:
            return GL_STENCIL_BUFFER_BIT;
    }
    return 0;
}

GLenum Translate(DepthFunc depth_func) {
    switch (depth_func) {
        case DepthFunc::Equal:
            return GL_EQUAL;
        case DepthFunc::NotEqual:
            return GL_NOTEQUAL;
        case DepthFunc::Greater:
            return GL_GREATER;
        case DepthFunc::GreaterEqual:
            return GL_GEQUAL;
        case DepthFunc::Less:
            return GL_LESS;
        case DepthFunc::LessEqual:
            return GL_LEQUAL;
    }
    return 0;
}

}  // namespace SD
