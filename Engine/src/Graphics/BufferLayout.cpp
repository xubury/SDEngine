#include <GL/glew.h>
#include "Graphics/BufferLayout.hpp"

namespace sd {

uint32_t getSizeOfType(GLenum type) {
    switch (type) {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
    }
    return 0;
}

VertexBufferLayout::VertexBufferLayout(uint32_t instanceStride)
    : m_stride(0), m_instanceStride(instanceStride) {}

void VertexBufferLayout::clear() {
    m_elements.clear();
    m_elements.shrink_to_fit();
}

const std::vector<VertexBufferLayoutElement> &VertexBufferLayout::getElements()
    const {
    return m_elements;
}

uint32_t VertexBufferLayout::getStride() const { return m_stride; }

uint32_t VertexBufferLayout::getInstanceStride() const {
    return m_instanceStride;
}

}  // namespace sd
