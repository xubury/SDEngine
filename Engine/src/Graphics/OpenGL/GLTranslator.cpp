#include "Graphics/OpenGL/GLTranslator.hpp"

namespace sd {

GLTranslator &GLTranslator::instance() {
    static GLTranslator s_instance;
    return s_instance;
}

GLenum GLTranslator::translate(BufferDataType dataType) {
    switch (dataType) {
        case BufferDataType::FLOAT:
            return GL_FLOAT;
        case BufferDataType::UINT:
            return GL_UNSIGNED_INT;
        case BufferDataType::UCHAR:
            return GL_UNSIGNED_BYTE;
    }
    return GL_INVALID_VALUE;
}

GLenum GLTranslator::translate(MeshTopology meshType) {
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
    return GL_INVALID_VALUE;
}

GLint GLTranslator::translate(BufferIOType ioType) {
    switch (ioType) {
        case BufferIOType::STATIC:
            return GL_STATIC_DRAW;
        case BufferIOType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
    }
    return GL_INVALID_VALUE;
}

GLenum GLTranslator::translate(TextureType textureType) {
    switch (textureType) {
        case TextureType::TEX_2D:
            return GL_TEXTURE_2D;
        case TextureType::TEX_3D:
            return GL_TEXTURE_3D;
        case TextureType::TEX_CUBE:
            return GL_TEXTURE_CUBE_MAP;
    }

    return GL_INVALID_VALUE;
}

GLenum GLTranslator::translate(TextureFormat textureFormat) {
    switch (textureFormat) {
        case TextureFormat::ALPHA:
            return GL_RED;
        case TextureFormat::RG:
            return GL_RG;
        case TextureFormat::RGB:
            return GL_RGB;
        case TextureFormat::RGBA:
            return GL_RGBA;
        case TextureFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        case TextureFormat::STENCIL:
            return GL_STENCIL_COMPONENTS;
    }
    return GL_INVALID_VALUE;
}

GLenum GLTranslator::translate(TextureFormatType textureFormatType) {
    switch (textureFormatType) {
        case TextureFormatType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case TextureFormatType::FLOAT:
            return GL_FLOAT;
    }

    return GL_INVALID_VALUE;
}

GLint GLTranslator::translate(TextureFormat textureFormat,
                              TextureFormatType textureFormatType) {
    switch (textureFormat) {
        case TextureFormat::ALPHA: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_R8;
                case TextureFormatType::FLOAT:
                    return GL_R32F;
            }
            break;
        }

        case TextureFormat::RG: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RG8;
                case TextureFormatType::FLOAT:
                    return GL_RG32F;
            }
            break;
        }

        case TextureFormat::RGB: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RGB8;
                case TextureFormatType::FLOAT:
                    return GL_RGB32F;
            }
            break;
        }

        case TextureFormat::RGBA: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_RGBA8;
                case TextureFormatType::FLOAT:
                    return GL_RGBA32F;
            }
            break;
        }

        case TextureFormat::DEPTH: {
            switch (textureFormatType) {
                case TextureFormatType::UBYTE:
                    return GL_DEPTH_COMPONENT;
                case TextureFormatType::FLOAT:
                    return GL_DEPTH_COMPONENT24;
            }
            break;
        }

        case TextureFormat::STENCIL: {
            return GL_STENCIL_COMPONENTS;
        }
    }

    return GL_INVALID_VALUE;
}

GLint GLTranslator::translate(TextureWrap textureWrap) {
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

GLint GLTranslator::translate(TextureFilter textureFilter) {
    switch (textureFilter) {
        case TextureFilter::NEAREST:
            return GL_NEAREST;
        case TextureFilter::LINEAR:
            return GL_LINEAR;
    }

    return GL_INVALID_VALUE;
}

GLint GLTranslator::translate(TextureMipmapFilter textureMipmapFilter) {
    switch (textureMipmapFilter) {
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

}  // namespace sd
