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

template <>
void VertexBufferLayout::push<float>(uint32_t count) {
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * getSizeOfType(GL_FLOAT);
}

template <>
void VertexBufferLayout::push<uint32_t>(uint32_t count) {
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count * getSizeOfType(GL_UNSIGNED_INT);
}

template <>
void VertexBufferLayout::push<uint8_t>(uint32_t count) {
    m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_stride += count * getSizeOfType(GL_UNSIGNED_BYTE);
}

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
