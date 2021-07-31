#include "Graphics/BufferLayout.hpp"

namespace sd {

uint32_t getSizeOfType(BufferDataType type) {
    switch (type) {
        case BufferDataType::FLOAT:
        case BufferDataType::UINT:
            return 4;
        case BufferDataType::UCHAR:
            return 1;
        default:
            return 0;
    }
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
