#include "Graphics/BufferLayout.hpp"

namespace SD {

static uint32_t GetBufferDataSize(BufferLayoutType type) {
    switch (type) {
        case BufferLayoutType::UByte:
            return 1;
        case BufferLayoutType::UInt:
        case BufferLayoutType::Int:
        case BufferLayoutType::Float:
            return 4;
        case BufferLayoutType::Float2:
            return 2 * 4;
        case BufferLayoutType::Float3:
            return 3 * 4;
        case BufferLayoutType::Float4:
            return 4 * 4;
        case BufferLayoutType::Mat4:
            return 4 * 4 * 4;
    }
    return 0;
}

static uint32_t GetComponentCount(BufferLayoutType type) {
    switch (type) {
        case BufferLayoutType::UByte:
        case BufferLayoutType::UInt:
        case BufferLayoutType::Int:
        case BufferLayoutType::Float:
            return 1;
        case BufferLayoutType::Float2:
            return 2;
        case BufferLayoutType::Float3:
            return 3;
        case BufferLayoutType::Float4:
            return 4;
        case BufferLayoutType::Mat4:
            return 4;
    }
    return 0;
}

VertexBufferLayout::VertexBufferLayout(uint32_t instance_stride)
    : m_stride(0), m_instance_stride(instance_stride) {}

void VertexBufferLayout::Push(BufferLayoutType type, bool normalized) {
    m_elements.push_back({type, GetComponentCount(type), m_stride, normalized});
    m_stride += GetBufferDataSize(type);
}

void VertexBufferLayout::Clear() {
    m_elements.clear();
    m_elements.shrink_to_fit();
    m_stride = 0;
}

const std::vector<VertexBufferLayoutElement> &VertexBufferLayout::GetElements()
    const {
    return m_elements;
}

uint32_t VertexBufferLayout::GetStride() const { return m_stride; }

}  // namespace SD
