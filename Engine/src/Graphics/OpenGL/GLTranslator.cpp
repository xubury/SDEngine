#include "Graphics/OpenGL/GLTranslator.hpp"

namespace sd {

GLTranslator &GLTranslator::instance() {
    static GLTranslator s_instance;
    return s_instance;
}

GLboolean GLTranslator::translate(Boolean boolean) {
    switch (boolean) {
        case Boolean::TRUE:
            return GL_TRUE;
        case Boolean::FALSE:
            return GL_FALSE;
        default:
            return GL_FALSE;
    }
}

GLenum GLTranslator::translate(DataType dataType) {
    switch (dataType) {
        case DataType::FLOAT:
            return GL_FLOAT;
        case DataType::UINT:
            return GL_UNSIGNED_INT;
        case DataType::UCHAR:
            return GL_UNSIGNED_BYTE;
        default:
            return GL_UNSIGNED_BYTE;
    }
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
        default:
            return GL_TRIANGLES;
    }
}

GLint GLTranslator::translate(BufferType bufferType) {
    switch (bufferType) {
        case BufferType::VERTEX:
            return GL_ARRAY_BUFFER;
        case BufferType::INDEX:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::UNIFORM:
            return GL_UNIFORM_BUFFER;
        default:
            return 0;
    }
}

GLint GLTranslator::translate(BufferIOType ioType) {
    switch (ioType) {
        case BufferIOType::STATIC:
            return GL_STATIC_DRAW;
        case BufferIOType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
        default:
            return 0;
    }
}

}  // namespace sd
